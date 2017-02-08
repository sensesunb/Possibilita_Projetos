### Rotina Básica Git

Este aquivo tem como objetivo expressar as rotinas basicas do git para adicionar e remover conteúdos de nosso repositório online.

#### Adiquirindo uma Cópia Local

No fluxo de trabalho padrão do Git, as suas alterações são realizadas em sua cópia local do repositório (ou "clone") e, posteriormente, enviadas ao repositório na web de nossos trabalhos.

Para obter uma cópia local de um reposistório, deve-se executar o seguinte comando:

  ```
  $ git clone /path/to/repository
  ```

#### Configurando Informações de Conta

Na primeira vez que utilizar as ferramentas git, será necessário estabelecer para o git qual o ** nome de usuário ** e ** email ** devem ser utilizados para registrar sua contribuição.

Os comandos:
```
$ git config --global user.name "Username"
$ git config --global user.email "your@email.com"
```
Realizam essa configuração e a tornam padrão para todos os seus repositórios.

#### Fluxo de Trabalho Git

##### 0. Verifique o Estado de sua Cópia

Antes de começar a trabalhar, verifique seo seu repositório local está atualizado em relação ao repositório na web.
Se outros colaboradores fizeram mudanças recentemente, é essencial conferir.

Para "puxar" as alterações mais recentes, realize o comando:
```
$ git pull
```
Suas credenciais GitHub serão requeridas.
Com a certeza de estar atualizado, podemos começar a trabalhar.

##### 1. Faça suas mudanças em seu computador

Altere, crie, ou remova os arquivos que achar necessário.
Todos os arquivos alterados em seu computador aparecerão destacadas ao chamar o comando:
```
$ git status
```
Este comando é essencial para manter controle do que está sendo alterado em sua cópia local.

##### 2. Envie suas Alterações

Para que suas mudanças locais sejam adicionadas ao repositório global é necessário:
* Indicar quais arquivos serão adicionados, pelo comando:
```
$ git add <filename>
```
* Anexar uma mensagem explicando suas mudanças
```
$ git commit -m "Commit message"
```
* "Empurrá-las" para o repositório online
```
$ git push
```

#### Referências
http://rogerdudler.github.io/git-guide/

https://www.thomas-krenn.com/en/wiki/Git_Basic_Commands
