//Hough Circles em Tempo real

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>


#include<iostream>
#include<stdint.h>
#include<Windows.h>
#include "conio.h"

using namespace cv;
using namespace std;

//Declaração das funções
int Analisa_Img_Inicial(cv::Mat, std::vector<cv::Vec3f>);
void Hough_Circles(cv::Mat &, std::vector<cv::Vec3f> &);
void Desenha_Circulos(cv::Mat &, std::vector<cv::Vec3f> &, Point);
void Classifica_circulos(cv::Mat &, std::vector<cv::Vec3f> &);
std::vector<cv::Vec3f> Ordena_centros(std::vector<cv::Vec3f> &);
void Desenha_Linhas(cv::Mat &, std::vector<cv::Vec3f> &);
Point Acha_MTQ(std::vector<cv::Vec3f> &);
void Desenha_Quadrado(cv::Mat &, Point);
void move_mouse(int);
void analisa_movimento(Point MTQ, Point MTQ_inicial, std::vector<cv::Vec3f> &);
void Janelas(cv::Mat &);
void clique_direito();
void clique_esquerdo();
void clique_duplo();
int calibragem2(Mat &, std::vector<cv::Vec3f> &, std::vector<cv::Vec3f> &, Point &); 
void func_trackbar(int, void*);
int acha_circulos(std::vector<cv::Vec3f> &, Mat &);
void temporiza_perda_circulos(int);


const int Treshold_max = 100;
int Treshold = 15;
cv::VideoCapture capWebcam(0);		     // declara um objeto da VideoCapture e associa a webcam, 0 => use a primeira webcam disponível
double fps;
double width;
double heigth;
int Conta_Erro_Testa  = 0;				 // conta a quantidade de quadros que o programa ficou sem achar círculo da testa
int Conta_Erro_Queixo = 0;				 // conta a quantidade de quadros que o programa ficou sem achar círculo do queixo 
int Conta_Erro_OmbroD = 0;				 // conta a quantidade de quadros que o programa ficou sem achar círculo do ombro direito
int Conta_Erro_OmbroE = 0;				 // conta a quantidade de quadros que o programa ficou sem achar círculo do ombro esquerdo
float acelerador = 1;
float passo = 0.1;

#define BIT_TESTA  0x01
#define BIT_QUEIXO 0x02
#define BIT_OMBROD 0x04
#define BIT_OMBROE 0x08

int main() {

	//criação de variáveis importantes
	int calibragem = 0;                      // variável usada para analisar a primeira imagem
	bool blnFrameReadSuccessfully;          // variável de controle pra saber se o frame foi lido corretamente
	cv::Mat ImgOriginal;					//Imagem obtida pela Webcam
	Point MTQ;								//Ponto medio entre circulo da testa e do queixo (será usado como base para a movimentação do mouse)
	Point MTQ_inicial;						//Ponto inicial usado pra comparar se houve movimentação 
	std::vector<cv::Vec3f> v3fCircles;	   //vetor de floats com 3 elementos, é o output da HoughCircles()
	std::vector<cv::Vec3f> v3fOrdenados;   //vetor contendo os círculos ordenados
	std::vector<cv::Vec3f> v3fOrdenados_inicial;
	char charCheckForEscKey = 0;
	int control = 0;
	int flag = 0;


	

	//Checa se a WebCam foi associada corretamente
	if (capWebcam.isOpened() == false) {				
		std::cout << "error: capWebcam not accessed successfully\n\n";	// se não, printa um mensagem de erro
		return(0);														// fecha 
	}


	cout << "Pressione barra de espaço quando os circulos estiverem no lugar certo\n";

	fps = capWebcam.get(CV_CAP_PROP_FPS);
	width = capWebcam.get(CV_CAP_PROP_FRAME_WIDTH);
	heigth = capWebcam.get(CV_CAP_PROP_FRAME_HEIGHT);

	

	//cout << "Frames per second using video.get(CV_CAP_PROP_FPS) : " << width << 
	//	" " << heigth << endl;

	cv::namedWindow("ImgOriginal", CV_WINDOW_AUTOSIZE);	// poderia ser CV_WINDOW_NORMAL pra ajustar o tamanho da janela
	createTrackbar("Tam. Quadrado", "ImgOriginal", &Treshold, Treshold_max, func_trackbar);

	
	while (charCheckForEscKey != 27 && capWebcam.isOpened()) {			// até apertar esc ou perder a conexão com a webcam

		 blnFrameReadSuccessfully = capWebcam.read(ImgOriginal);		// pega o proximo frame

		if (!blnFrameReadSuccessfully || ImgOriginal.empty()) {		// se o frame não for lido corretamente
			std::cout << "error: frame not read from webcam\n";		// printa erro
			break;													// sai do loop
		}

			//loop de calibração
			while(control == 0) {
				
				control = calibragem2(ImgOriginal, v3fCircles, v3fOrdenados_inicial, MTQ_inicial);

				if (control == -1) {
					capWebcam.release();
					return 0;
				}
				
			};

			if (control == 1) {

				//Hough_Circles(ImgOriginal, v3fCircles);
				v3fOrdenados = Ordena_centros(v3fOrdenados_inicial);
				control++;
			}

	
		
		flag = acha_circulos(v3fOrdenados, ImgOriginal);
		
		
			cout << "Resultado do acha circulos = " << flag << endl;
		
	    temporiza_perda_circulos(flag);
		MTQ = Acha_MTQ(v3fOrdenados);
		Desenha_Linhas(ImgOriginal, v3fOrdenados);
		Desenha_Circulos(ImgOriginal, v3fOrdenados, MTQ);			//Chama função pra desenhar na imagem os círculos encontrados
		Desenha_Quadrado(ImgOriginal, MTQ_inicial);
		analisa_movimento(MTQ, MTQ_inicial, v3fOrdenados);
		Janelas(ImgOriginal);
		clique_direito();
		clique_duplo();
		clique_esquerdo();
		
		if ((Conta_Erro_OmbroD > 180) || (Conta_Erro_OmbroE > 180) || (Conta_Erro_Queixo > 180) || (Conta_Erro_Testa > 180)) {
			control = 0;
			cout << "Por favor, recalibre\n";
		}

		charCheckForEscKey = cv::waitKey(1);			//delay (em ms) e pega alguma tecla pressionada
	}	// fim do while

	
	capWebcam.release();

	return(0);
}

/*********************************************************************************************
	Função de calibragem que interage com o usuário para que ele indique se os círculos
na imagem original são os círculos esperados.

	-Parâmetros de entrada:
		.Classe do tipo Mat que armazena a imagem obtida pela WebCam;

	-Parâmetros de saída:
		.Inteiro que representa a interação com o usuário:
			0 - indica erro;
			1 - indica que foram encontrados os círculos corretos.
*********************************************************************************************/
	int Analisa_Img_Inicial(cv::Mat Imagem_calibrada, std::vector<cv::Vec3f> centros){
		int flag;
		char resposta;

		

		cout << "A imagem marca os círculos corretos e conectados da forma correta?\n"
			 << "Digite [s] para responder sim e [n] para responder não";
				
		cin >> resposta;
		
		if (resposta == 's') {
			flag = 1;
		}
		else {
			flag = 0;
		}

		return flag;
	}

	/*********************************************************************************************
	Função que passa um filtro na imagem original com objetivo de diminuir a detecção de circulos
indesejáveis, e depois roda o algoritmo HoughCircles pra encontrar os círculos.

	-Parâmetros de entrada:
		.Endereço de uma classe do tipo Mat que armazena a imagem obtida pela webcam; 
		.Endereço de uma classe do tipo Mat que armazena a imagem na qual o HoughCircles irá
tralhar, ou seja, imagem obtida após a utilização de filtros pra preparar a imagem;
		.Endereço de um vetor de 3 floats (saída do HoughCircles), que vai armazenar o centro e o
raio dos círculos da última imagem analisada.

	-Parâmetros de saída:
		.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/
	void Hough_Circles(cv::Mat &ImgOriginal, std::vector<cv::Vec3f> &v3fCircles) {

		cv::Mat Img_Final;					    //Imagem que será usada para achar os círculos

		cvtColor(ImgOriginal, Img_Final, CV_BGR2GRAY);				 //Transforma a imagem em preto e branco pra facilitar a detecção de círculos
		GaussianBlur(Img_Final, Img_Final, cv::Size(9, 9), 2, 2);    //Reduz o ruído pra evitar detecção errada de círculos

																	 // preenche o vetor de circulos com todos os circulos na imagem processada
		cv::HoughCircles(Img_Final,			    // imagem de entrada
			v3fCircles,							// saída da função (tem que ser um vetor padrão da biblioteca)
			CV_HOUGH_GRADIENT,					// algoritmo para detectar circulos 
			2,									// tamanho da imagem dividido por esse numero, no caso 2
			Img_Final.rows / 4,				    // distância mínima em pixels entre os centros dos circulos detectados
			200,								// threshold máximo do detector Canny edge detector (chamado por cvHoughCircles)						
			50,									// threshold mínimo do Canny edge detector (setado em metade do valor anterior)
			10,									// raio mínimo do circulo (qualquer circulo com raio menor não vai ser detectado)
			40);								// raio máximo do círculo (qualquer círculo com raio maior não vai ser detectado)

		return;
	}

	/*********************************************************************************************
	Função que desenha os círculos encontrados na imagem obtida pela WebCam.

	-Parâmetros de entrada:
		.Endereço de uma classe do tipo Mat que armazena a imagem obtida pela webcam;
		.Endereço de um vetor de 3 floats (saída do HoughCircles).

	-Parâmetros de saída:
		.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/
	void Desenha_Circulos(cv::Mat &ImgOriginal, std::vector<cv::Vec3f> &v3fCircles, Point MTQ){
	
		for (int i = 0; i < v3fCircles.size() ; i++) {		// para cada círculo (no máximo 4 círculos)
													// mostra posição x, y da bola e raio para a linha de comando
	//		std::cout << "ball " << i + 1 << " position x = " << v3fCircles[i][0]   // posição x do ponto central do círculo
	//			<< ", y = " << v3fCircles[i][1]								    // posição y do ponto central do círculo
	//			<< ", radius = " << v3fCircles[i][2] << "\n";				    // raio do círculo

																				// desenha pequeno circulo verde no centro do objeto detectado
			cv::circle(ImgOriginal,												// desenha na imagem original
				cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]),		// ponto central do circulo
				3,																// raio do circulo em pixels
				cv::Scalar(0, 255, 0),											// desenha verde puro (***BGR, não RGB)
				CV_FILLED);														// espessura, preenche o circulo

																				// desenha pequeno circulo vermelho no centro do objeto detectado
			cv::circle(ImgOriginal,												// desenha na imagem original
				cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]),		// ponto central do circulo
				(int)v3fCircles[i][2],											// raio do circulo em pixels
				cv::Scalar(255, 0, 0),											// desenha vermelho puro (***BGR, não RGB)
				3);																// espessura, preenche o circulo
		}	// fim do for

		//Desenha o ponto MTQ
		cv::circle(ImgOriginal,												// desenha na imagem original
			cv::Point((int) MTQ.x, (int)MTQ.y),								// ponto central do circulo
			3,																// raio do circulo em pixels
			cv::Scalar(0, 0, 0),											// desenha preto
			CV_FILLED);														// espessura, preenche o circulo
	}

	/*********************************************************************************************
	Função que d

	-Parâmetros de entrada:
	.

	-Parâmetros de saída:
	.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/
	void Classifica_circulos(cv::Mat &ImgOriginal, std::vector<cv::Vec3f> &v3fCircles) {

		std::vector<cv::Vec3f> centros_ordenados;		//vetor de apoio que armazena os círculos ordenados pela proximidade do branco
		std::vector<float> ordenacao;					//vetor que armazena a prox_branco de cada círculo
		float prox_branco;								//variável que indica o quão próximo do branco o círculo é
		int x1, x2, y1, y2;								//variaveis que armazenam os limites pra cortar a img
		float raio;										//raio do círculo a ser analisado 
		float Num_pixels;									//armazena a quantidade de pixels que tem na imagem cortada
		int Branco_perfeito;							//soma de todos os pixels da img cortada como se eles fossem branco puro
		int Qtd_branco_Img = 0;								//soma de todos os pixels da img cortada
		cv::Mat Img_cinza;								//imagem em preto e branco
		cv::Mat Img_cortada;							//imagem cortada da cinza que tem apenas o círculo a ser trabalhado

		int  max, cc;



		cvtColor(ImgOriginal, Img_cinza, CV_BGR2GRAY);				 //Transforma a imagem em preto e branco

		//loop para calcular o quao proximo branco é um círculo
		for (int i = 0; i < v3fCircles.size(); i++) {
			raio = v3fCircles[i][2];     //trabalha apenas com a parte inteira do raio (sempre arredondando para baixo), pois a função de cortar a img exige entrada inteira

			//define os limites pra cortar a img
			x1 = (int) (v3fCircles[i][0] - (raio/1.45));
			x2 = (int) (v3fCircles[i][0] + (raio/1.45));
			y1 = (int) (v3fCircles[i][1] - (raio/1.45));
			y2 = (int) (v3fCircles[i][1] + (raio/1.45));

			//Corta a imagem para analisar apenas um quadrado dentro do círculo
			if ((x1 > 0) && (x2 < 640)) {
				Img_cortada = Img_cinza.colRange(x1, x2);       //corta a imagem em colunas

				if ((y1 > 0) && (y2 < 480))
					Img_cortada = Img_cortada.rowRange(y1, y2);	  //corta a imagem em linhas
			}

			Num_pixels = (float) ((y2 - y1) * (x2 - x1));        // calculo do numero de pixels que existem na imagem cortada, levando em conta que o raio do círculo é igual
											     //metade da diagonal de um quadrado inscrito no círculo.

			Branco_perfeito = 255 * Num_pixels;		//leva em consideração que todos os pixels da img cortada sejam brancos puros, ou seja, armazenem o valor 255

			max = 0;
			cc = 0;
			//loop para percorrer todos os pixels da img cortada e soma-los, tendo assim um valor que vai nos indicar o quão próxima a nossa img cortada é do branco perfeito	
			for (int k = 0; k < Img_cortada.rows; k++) {
				for (int j = 0; j < Img_cortada.cols; j++) {
				  Qtd_branco_Img = (Qtd_branco_Img + Img_cortada.at<uint8_t>(k, j));
				  cc++;
				  
				  if (Img_cortada.at<uint8_t>(k, j)> max) max = Img_cortada.at<uint8_t>(k, j);
				}
			}

			prox_branco = (float) Qtd_branco_Img / Branco_perfeito;

			if (prox_branco > 1.1) {

				cout << "\nNum pixels  e = " << Num_pixels << "\n";
				//	cout << "\nNum linhas  e = " << Img_cortada.rows << "\n";
				//	cout << "\nNum colunas  e = " << Img_cortada.cols << "\n";
					//cout << "\nQuantidade de branco  e = " << Qtd_branco_Img << "\n";
				cout << "\ncc  e = " << cc << "\n";

				std::cout << "ball " << i + 1 << " position x = " << v3fCircles[i][0]   // posição x do ponto central do círculo
					<< ", y = " << v3fCircles[i][1]								    // posição y do ponto central do círculo
					<< ", radius = " << v3fCircles[i][2] << "\n";				    // raio do círculo

				cout << "\nA proximidade do branco e = " << prox_branco << "\n";
			}
			//ordenacao[i] = prox_branco;
		}

		//
		return;
	}
	
	/*********************************************************************************************
	Função para ordenar o vetor que contém os centros e os raios dos círculos, de forma que as 
posições do vetor indicam qual é o círculo seguindo a oranização abaixo:
		
		Posição 0 -> Círculo da testa;
		Posição 1 -> Círculo do queixo;
		Posição 2 -> Círculo do ombro esquerdo;
		Posição 3 -> Círculo do ombro direito;

	-Parâmetros de entrada:
	.

	-Parâmetros de saída:
	.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/

	std::vector<cv::Vec3f> Ordena_centros(std::vector<cv::Vec3f> &v3fCircles) {

		std::vector<cv::Vec3f> centros_ordenados;		//vetor de apoio pra organizar os círculos
		int indT = 0;									//indice do circulo da testa
		int indQ = 0;									//indice do circulo do queixo
		int indOE = 0;									//indice do circulo do ombro esquerdo
		int indOD = 0;									//indice do circulo do ombro direito

		if (v3fCircles.size() < 4) {
			//cout << "\nNumero errado de circulos";
			return v3fCircles;
		}

		//cria o vetor centros ordenados com todas as posições que pode-se precisar setadas em 0
		centros_ordenados.push_back(NULL);
		centros_ordenados.push_back(NULL);
		centros_ordenados.push_back(NULL);
		centros_ordenados.push_back(NULL);


		//laço para encontrar o circulo da testa.
		for (int i = 0; i < v3fCircles.size(); i++) {
			if (v3fCircles[i][1] <= v3fCircles[indT][1]) {			//condição para achar o ponto com menor y
				indT = i;			
				centros_ordenados[0][0] = v3fCircles[indT][0];
				centros_ordenados[0][1] = v3fCircles[indT][1];
				centros_ordenados[0][2] = v3fCircles[indT][2];
			}
		}
		
		v3fCircles[indT][0] = 641;									//elimina as chances desse ponto ser pego em outro laço
		v3fCircles[indT][1] = 481;									//elimina as chances desse ponto ser pego em outro laço
		
		//laço para encontrar o circulo da queixo. (considerando que o do queixo passa a ser o ponto mais alto depois do da testa)
		for (int i = 0; i < v3fCircles.size(); i++) {
			if (v3fCircles[i][1] <= v3fCircles[indQ][1]) {			//condição para achar o ponto com menor y
				indQ = i;
				centros_ordenados[1][0] = v3fCircles[indQ][0];
				centros_ordenados[1][1] = v3fCircles[indQ][1];
				centros_ordenados[1][2] = v3fCircles[indQ][2];
			}
		}

		v3fCircles[indQ][0] = 641;									//elimina as chances desse ponto ser pego em outro laço
		v3fCircles[indQ][1] = 481;									//elimina as chances desse ponto ser pego em outro laço
		

		//laço pra encontrar circulo do ombro esquerdo
		for (int i = 0; i < v3fCircles.size(); i++) {
			if (v3fCircles[i][0] <= v3fCircles[indOE][0]) {			//condição para achar o ponto com menor x
				indOE = i;
				centros_ordenados[2][0] = v3fCircles[indOE][0];
				centros_ordenados[2][1] = v3fCircles[indOE][1];
				centros_ordenados[2][2] = v3fCircles[indOE][2];
			}
		}

		v3fCircles[indOE][0] = 641;									//elimina as chances desse ponto ser pego em outro laço
		v3fCircles[indOE][1] = 481;									//elimina as chances desse ponto ser pego em outro laço


		//laço pra encontrar circulo do ombro direito
		for (int i = 0; i < v3fCircles.size(); i++) {
			if (v3fCircles[i][0] <= v3fCircles[indOD][0]) {			//condição para achar o ponto com menor x
				indOD = i;
				centros_ordenados[3][0] = v3fCircles[indOD][0];
				centros_ordenados[3][1] = v3fCircles[indOD][1];
				centros_ordenados[3][2] = v3fCircles[indOD][2];
			}
		}

		v3fCircles[indOD][0] = 641;									//elimina as chances desse ponto ser pego em outro laço
		v3fCircles[indOD][1] = 481;									//elimina as chances desse ponto ser pego em outro laço

		//cout << "\nO circulo da testa esta na posicao (" << centros_ordenados[0][0] << "," << centros_ordenados[0][1] << ")";
	//	cout << "\nO circulo do queixo esta na posicao (" << centros_ordenados[1][0] << "," << centros_ordenados[1][1] << ")";
		//cout << "\nO circulo do ombro esquerdo esta na posicao (" << centros_ordenados[2][0] << "," << centros_ordenados[2][1] << ")";
		//cout << "\nO circulo do ombro direito esta na posicao (" << centros_ordenados[3][0] << "," << centros_ordenados[3][1] << ")";

		return  centros_ordenados;
	}
	/*********************************************************************************************
	Função que d

	-Parâmetros de entrada:
	.

	-Parâmetros de saída:
	.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/
	void Desenha_Linhas(cv::Mat &ImgOriginal, std::vector<cv::Vec3f> &v3fCircles) {
		
		if (v3fCircles.size() < 4) {
		//	cout << "\nNumero errado de circulos";
			return;
		}

		Point testa;
		Point queixo;
		Point OmbroE;
		Point OmbroD;


		//atualiza pontos
		testa.x = (int) v3fCircles[0][0];
		testa.y = (int) v3fCircles[0][1];

		queixo.x = (int) v3fCircles[1][0];
		queixo.y = (int) v3fCircles[1][1];

		OmbroE.x = (int) v3fCircles[2][0];
		OmbroE.y = (int) v3fCircles[2][1];

		OmbroD.x = (int) v3fCircles[3][0];
		OmbroD.y = (int) v3fCircles[3][1];

		 line(ImgOriginal, testa, queixo, Scalar(0, 0, 255), 2, 8, 0);				//Desenha linha da testa ate o queixo

		 line(ImgOriginal, OmbroE, OmbroD, Scalar(0, 0, 255), 2, 8, 0);				//Desenha lihna entre os ombros 
		
		return;
	}
	/*********************************************************************************************
	Função que acha o ponto medio entre os círculos da testa e do queixo.

	-Parâmetros de entrada:
	.

	-Parâmetros de saída:
	.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/
	Point Acha_MTQ(std::vector<cv::Vec3f> &v3fCircles) {

		Point media;

		if (v3fCircles.size() < 4) {
			//	cout << "\nNumero errado de circulos";
			return media;
		}

		media.x = (int) ((v3fCircles[0][0] + v3fCircles[1][0]) / 2);
		media.y = (int) ((v3fCircles[0][1] + v3fCircles[1][1]) / 2);

		return media;
	}

	/*********************************************************************************************
	Função que acha o ponto medio entre os círculos da testa e do queixo.

	-Parâmetros de entrada:
	.

	-Parâmetros de saída:
	.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/
	
	void Desenha_Quadrado(cv::Mat &ImgOriginal, Point MTQ) {

		double t = 0.15;     					//Treshold para o movimeno do mouse, predefinida como 10;
		int mx, my;								//Margem do quadrado de movimento
		Point a, b;								//Pontos opostos que determinam as extremidades do quadrado

		t = (double) Treshold / Treshold_max;

		mx = (int) ((640 * t) / 2);
		my = (int) ((480 * t) / 2);
	
		a.x = MTQ.x + mx;
		a.y = MTQ.y + my;

		b.x = MTQ.x - mx;
		b.y = MTQ.y - my;

		rectangle(ImgOriginal, a, b, Scalar(0, 0, 255), 1, 8, 0);

		return;

	}

	/*********************************************************************************************
	Função que acha o ponto medio entre os círculos da testa e do queixo.

	-Parâmetros de entrada:
	.

	-Parâmetros de saída:
	.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/

	void move_mouse(int direction){
		
		POINT p;
		int x, y;

		if (GetCursorPos(&p))
		{
			x = p.x;
			y = p.y;
		}

		switch (direction)
		{
		case 0: // baixo
			y = y - acelerador;
			break;
		case 1: // cima
			y = y + acelerador;
			break;
		case 2: // direita
			x = x + acelerador;
			break;
		case 3: // esquerda
			x = x - acelerador;
			break;
		}

		SetCursorPos(x, y);
	}

	/*********************************************************************************************
	Função que acha o ponto medio entre os círculos da testa e do queixo.

	-Parâmetros de entrada:
	.

	-Parâmetros de saída:
	.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/

	void analisa_movimento(Point MTQ, Point MTQ_inicial, std::vector<cv::Vec3f> &v3fCircles) {
		
		Point movimento;
		double t;

		t = (double) Treshold / Treshold_max;

		if (v3fCircles.size() != 4) {
			return;
		}

		movimento.x = 0;
		movimento.y = 0;

		//Analisa movimento pra direita
		if ((MTQ.x < MTQ_inicial.x) && (abs(MTQ_inicial.x - MTQ.x) > 320 * t)) {
			movimento.x = 1;
		}

		//Analisa movimento pra esquerda
		if ((MTQ.x > MTQ_inicial.x) && (abs(MTQ_inicial.x - MTQ.x) > 320 * t)) {
			movimento.x = -1;
		}
		
		//Analisa movimento pra baixo
		if ((MTQ.y > MTQ_inicial.y) && (abs(MTQ_inicial.y - MTQ.y) > 240 * t)) {
			movimento.y = -1;
		}
		
		//Analisa movimento pra cima
		if ((MTQ.y < MTQ_inicial.y) && (abs(MTQ_inicial.y - MTQ.y) > 240 * t)) {
			movimento.y = 1;
		}

				//loop para movimentar eixo x
				switch (movimento.x) {
				
				case 1:
					//std::cout << "left" << std::endl;
					move_mouse(2);
					break;
				case -1:
					//std::cout << "right" << std::endl;
					move_mouse(3);
					break;

				default:
					break;
				}

				//loop para movimentar eixo y
				switch (movimento.y) {

				case -1:
					//std::cout << "down" << std::endl;
					move_mouse(1);
					break;
				case 1:
					//std::cout << "up" << std::endl;
					move_mouse(0);

					break;
				default:
					break;
				}
				
				if ((movimento.x != 0) || (movimento.y != 0)) {
					if (acelerador < 5) {
						acelerador = acelerador + passo;
					}
				}
				else {
					acelerador = 1;
				}

		return;
	}

	/*********************************************************************************************
	Função que acha o ponto medio entre os círculos da testa e do queixo.

	-Parâmetros de entrada:
	.

	-Parâmetros de saída:
	.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/
	void Janelas(cv::Mat &ImgOriginal) {

		// mostra janelas
		flip(ImgOriginal, ImgOriginal, 1);
		cv::imshow("ImgOriginal", ImgOriginal);									

		return;
	}
	/*********************************************************************************************
	Função que acha o ponto medio entre os círculos da testa e do queixo.

	-Parâmetros de entrada:
	.

	-Parâmetros de saída:
	.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/
	void clique_direito() {

		INPUT Input = { 0 };

		if ((Conta_Erro_OmbroD > 30) && (Conta_Erro_OmbroE < 10)) {
			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			::SendInput(1, &Input, sizeof(INPUT));
		
			// right up
			::ZeroMemory(&Input, sizeof(INPUT));
			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			::SendInput(1, &Input, sizeof(INPUT));
			Conta_Erro_OmbroD = 0;
		}

	}
	
	void clique_esquerdo() {

		INPUT Input = { 0 };

		if ((Conta_Erro_OmbroE > 30) && (Conta_Erro_OmbroD < 10)) {
			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			::SendInput(1, &Input, sizeof(INPUT));
			Conta_Erro_OmbroE = -1200;					//controle pra pessoa arrastar algum objeto
		}

		if (Conta_Erro_OmbroE == 0) {
			// left up
			::ZeroMemory(&Input, sizeof(INPUT));
			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			::SendInput(1, &Input, sizeof(INPUT));
		}
	}
	/*********************************************************************************************
	Função que acha o ponto medio entre os círculos da testa e do queixo.

	-Parâmetros de entrada:
	.

	-Parâmetros de saída:
	.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/
	void clique_duplo() {

		INPUT Input = { 0 };

		if ((Conta_Erro_OmbroE > 30) && (Conta_Erro_OmbroD > 30)) {
			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			::SendInput(1, &Input, sizeof(INPUT));

			// left up
			::ZeroMemory(&Input, sizeof(INPUT));
			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			::SendInput(1, &Input, sizeof(INPUT));

			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			::SendInput(1, &Input, sizeof(INPUT));

			// left up
			::ZeroMemory(&Input, sizeof(INPUT));
			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			::SendInput(1, &Input, sizeof(INPUT));
			Conta_Erro_OmbroE = 0;
			Conta_Erro_OmbroD = 0;
		}
	}
	/*********************************************************************************************
	Função que acha o ponto medio entre os círculos da testa e do queixo.

	-Parâmetros de entrada:
	.

	-Parâmetros de saída:
	.Não há, pois a função altera as variáveis direto no endereço de memória delas.

	*********************************************************************************************/
	
	int calibragem2(Mat &ImgOriginal, std::vector<cv::Vec3f> &v3fCircles, std::vector<cv::Vec3f> &v3fOrdenados_inicial, Point &MTQ_inicial)
	{

		int control = 0;
		char charCheckForSpaceKey = 0;

		capWebcam.read(ImgOriginal);

		Hough_Circles(ImgOriginal, v3fCircles);
		v3fOrdenados_inicial = Ordena_centros(v3fCircles);
		MTQ_inicial = Acha_MTQ(v3fOrdenados_inicial);
		Desenha_Linhas(ImgOriginal, v3fOrdenados_inicial);
		Desenha_Circulos(ImgOriginal, v3fOrdenados_inicial, MTQ_inicial);
		Desenha_Quadrado(ImgOriginal, MTQ_inicial);
		Janelas(ImgOriginal);

		charCheckForSpaceKey = waitKey(10);

		if ((v3fOrdenados_inicial.size() == 4) && (charCheckForSpaceKey == 32)) {
			control = 1;
		}

		if (charCheckForSpaceKey == 27) {
			control = -1;
		}

		return control;

	}


	void func_trackbar(int , void* ) {

		double v1;

		v1 = (double) Treshold / Treshold_max;
	
	}

	int acha_circulos(std::vector<cv::Vec3f> &v3fOrdenados, Mat &ImgOriginal) {

		vector<Vec3f> Aux;
		Mat Img_cortada;
		int x1, x2, y1, y2;
		int i;
		float raio;
		int bitmap_erro = 0;

		if (v3fOrdenados.size() != 4) {
			return -1;
		}


		for (i = 0; i < 4; i++) {
			
			raio = v3fOrdenados[i][2];

			//define os limites pra cortar a img
			x1 = (int)(v3fOrdenados[i][0] - (2 * raio));
			x2 = (int)(v3fOrdenados[i][0] + (2 * raio));
			y1 = (int)(v3fOrdenados[i][1] - (2 * raio));
			y2 = (int)(v3fOrdenados[i][1] + (2 * raio));

			//Controle para não ultrapassar as bordas da imagem 
			if (x1 < 1) {
				x1 = 1;
			}

			if (x2 > 639) {
				x2 = 639;
			}

			if (y1 < 1) {
				y1 = 1;
			}

			if (y2 > 479) {
				y2 = 479;
			}

			//Corta a imagem para analisar apenas um quadrado dentro do círculo
			if ((x1 > 0) && (x2 < 640)) {
				Img_cortada = ImgOriginal.colRange(x1, x2);       //corta a imagem em colunas

				if ((y1 > 0) && (y2 < 480)) {

					Img_cortada = Img_cortada.rowRange(y1, y2);	  //corta a imagem em linha
					Hough_Circles(Img_cortada, Aux);			//Chama função pra encontrar os circulos 
					
				}
			}


			if (Aux.size() == NULL) {
				bitmap_erro = bitmap_erro | (1 << i);
			}
			else if (Aux.size() == 1) {


				v3fOrdenados[i][0] = Aux[0][0] + x1;
				v3fOrdenados[i][1] = Aux[0][1] + y1;
				v3fOrdenados[i][2] = Aux[0][2];
/*
				cout << "x1 = " << x1
					<< " x2 = " << x2
					<< " y1 = " << y1
					<< " y2 = " << y2
					<< " raio = " << raio
					<< " Aux.size = " << Aux.size()
					<< " Aux[i][0] = " << Aux[0][0] 
					<< " Aux[i][1] = " << Aux[0][1]
					<< " Aux[i][2] = " << Aux[0][2]
					<< " Cortou a imagem"
					<< endl;
					*/
			}
			else {
				return -2;
			}


		}

		return bitmap_erro;
	}

	void temporiza_perda_circulos(int erro) {

		if (erro & BIT_TESTA) {
			Conta_Erro_Testa++;
		}
		else {
			Conta_Erro_Testa = 0;
		}

		if (erro & BIT_QUEIXO) {
			Conta_Erro_Queixo++;
		}
		else {
			Conta_Erro_Queixo = 0;
		}

		if (erro & BIT_OMBROD) {
			Conta_Erro_OmbroD++;
		}
		else {
			Conta_Erro_OmbroD = 0;
		}

		if (erro & BIT_OMBROE) {
			Conta_Erro_OmbroE++;
		}
		else {
			Conta_Erro_OmbroE = 0;
		}
		
		if (Conta_Erro_OmbroD + Conta_Erro_OmbroE + Conta_Erro_Queixo + Conta_Erro_Testa > 10) {
			cout << "Erro Testa = " << Conta_Erro_Testa << endl
				<< "Erro Queixo = " << Conta_Erro_Queixo << endl
				<< "Erro Ombro direito = " << Conta_Erro_OmbroD << endl
				<< "Erro Ombro Esquerdo = " << Conta_Erro_OmbroE << endl;
		}
	}