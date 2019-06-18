//---------------------------------------------------------------------------

#include <vcl.h>	
#pragma hdrstop

#include "mna1gr1.h"
#include "menuParametros.h"
#include "menuPlot.h"
#include "menuSobre.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
TForm2 *Form2;
TForm3 *Form3;
TForm4 *Form4;
/* 
* UNIVERSIDADE FEDERAL DO RIO DE JANEIRO
* DEPARTAMENTO DE ELETRONICA E DE COMPUTACAO
* DISCIPLINA: CIRCUITOS ELETRICOS II 
* TURMA: 2016/1
* AUTORES: Rebeca Araripe
*	    Renata Baptista
*	    Vinicius Mesquita
*
* Descricao:
*  programa de analise nodal modificada e resposta em frequencia
*
*/

/*
Elementos aceitos e linhas do netlist:

Elementos aceitos e linhas do netlist:
Resistor:  R<nome> <no+> <no-> <resistência>
VCCS:      G<nome> <io+> <io-> <vi+> <vi-> <transcondutância>
VCVC:      E<nome> <vo+> <vo-> <vi+> <vi-> <ganho de tensão>
CCCS:      F<nome> <io+> <io-> <ii+> <ii-> <ganho de corrente>
CCVS:      H<nome> <vo+> <vo-> <ii+> <ii-> <transresistência>
Fonte I:   I<nome> <io+> <io-> <módulo> [<fase em graus> valor contínuo]
Fonte V:   V<nome> <vo+> <vo-> <módulo> [<fase em graus> valor contínuo]
Capacitor: C<nome> <no+> <no-> <capacitância>
Indutor:   L<nome> <no+> <no-> <indutância>
Amplificador operacional:  O<nome> <vo+> <vo-> <vi-> <vi+>
Ind. mútua:K<nome> <L1> <L2> <coeficiente de acoplamento>
Transistor MOS: M<nome> <nód> <nóg> <nós> <nób> <tipo> [L=<comprimento> W=<largura>]
[<K> <VT0> <Lambda> <Gama> <Fi> <Ld>]
Resistor não linear por partes: N<nome> <nó+> <nó-> <4 pontos vi ji >
Diodo:     D<nome> <nó+> <nó-> [<Is> <VT>]
Amplificador: A<nome> <vo+> <vo-> <vi-> <vi+> <GB> <polo> <resistência de saída>
Linha de transmissão LC: T<nome> <v1+> <v-> <v2+> <Z0> <atraso>
Comentário: *<comentário>
Comando: .AC <LIN ou OCT ou DEC> <pontos> <início> <fim>
O número de pontos é por década ou por oitava com as opções DEC (ou LOG) e OCT.
Senão é o total com a opção LIN. As frequências são em Hz por padrão.
*/


// Bibliotecas
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include <complex.h>

//Macros
#define _MY_DEBUG_
#define COMPRIMENTO_MAX_NOME_ARQUIVO			  255
#define EOS						  '\0'
#define MAX_ELEMENTOS					    50
#define MAX_ERRO_DEFAULT					 1e-6
#define MAX_ERRO_RELATIVO				1e-9
#define MAX_INIT 					200
#define MAX_ITERACOES					500
#define MAX_LINHA					80
#define MAX_NOME					11
#define MAX_NOS						50
#define MAX_TIPO_TRANS					4
#define	PI						(4*atan(1))
#define RESISTENCIA_ABERTO				1e9
#define RESISTENCIA_CURTO				1e-9
#define SEM_TRANSISTOR					66
#define SEM_REATIVO					67
#define TEM_REATIVO					-22
#define TOLERANCIA				  1e-12
#define TOLG 																1e-12
#define versao  													"10-07-2016"
#define I                                                  cppcomplex(0.0,1.0)
#define X									1


//codigos de erro
#define OK						  0
#define NOME_ARQUIVO_NULO				  1
#define ERRO_ABRINDO_ARQUIVO				  2
#define NUMERO_MAXIMO_ELEMENTOS				  3
#define ELEMENTO_DESCONHECIDO				  4
#define TIPO_TRANS_INVALIDO				  5
#define NUMERO_MAX_CORRENTES				  6
#define NUMERO_MAX_NOS					  7
#define TIPO_ESCALA_INVALIDO				  8
#define NUMERO_VARIAVEIS_INVALIDO			  9
#define SAIDA_VOLUTANRIA				  10
#define NAO_CONVERGIU					  11
#define CONVERGIU								12
#define ARQUIVO_CORROMPIDO			13
#define SISTEMA_SINGULAR				14





// XPRINTF
#define printf xprintf
void xprintf(char* format,...) /* Escreve no memo1 */
{
	va_list paramlist;
	char txt[2000];
	va_start(paramlist,format);
	vsprintf(txt,format,paramlist);
	Form1->Memo1->Lines->Add(txt);
}



//tipos

typedef std::complex<double> cppcomplex;

typedef struct elemento
{ 
	char nome[MAX_NOME];
	double valor;
	double m;
	double fase; 
	cppcomplex valorAC;
	double gmGS, gmDS, gmBS, correnteDCmos;
	int a, b ,c ,d ,g ,s ,x ,y, par, modoOperacao, inverteu;
	char tipoTrans[MAX_TIPO_TRANS];
	double comprimentoTrans;
	double larguraTrans;
	double kTrans;
	double vtzeroTrans;
	double lambdaTrans;
	double gamaTrans;
	double phiTrans;
	double ldTrans;
	double Cgs, Cgd, Cbg, vt, vDS, vGS, vBS;
} elemento;

typedef enum
{
	LIN,
	OCT,
	DEC	
} tipoEscala;


typedef struct parametrosAnaliseAC
{ /* Parametros analise AC */
	tipoEscala escala;
	double numPontos;
	double comeco;
	double fim;	
} parametrosAnaliseAC;





// form1
char mostraMOS;
char mostraEstampas_PO;
char mostraEstampa_JW;
char mostraResultados;
char mostraResultadosAndErros;
char salvaPO;
char salvaParamTrans;
char usarResistenciaAberto;

//form 2
double MAX_ERRO;
double aproximacaoInicialDasCorrentes;

//form 3
parametrosAnaliseAC analiseAC;





//prototipos
int
lerNetlist(char listaDeNos[MAX_NOS + 1][MAX_NOME + 2], char *, int *, int *, int *, elemento [MAX_ELEMENTOS], parametrosAnaliseAC * , int*);

int
escreverTabela (char listaDeNos[MAX_NOS + 1][MAX_NOME + 2], int , int, cppcomplex sistema[MAX_NOS+1][MAX_NOS+2], int * , double *, char * , elemento [MAX_ELEMENTOS]);

int
escreverTabelaPO (char listaDeNos[MAX_NOS + 1][MAX_NOME + 2], double sistema[MAX_NOS+1][MAX_NOS+2], int *,  char * , elemento netlistLido[MAX_ELEMENTOS]);

int 
controleDosNos(char *, char [MAX_NOS + 1][MAX_NOME + 2], int * );

int
printaNetlistFinalizado(char listaDeNos[MAX_NOS + 1][MAX_NOME + 2], elemento netlist[MAX_ELEMENTOS], int, int);

int
acharElemento(char*, int*, elemento netlist[MAX_ELEMENTOS] , int);

int
montaSNM_PO(double Yn[MAX_NOS+1][MAX_NOS+2], elemento netlistLido[MAX_ELEMENTOS], int * , int * , int * , double solucaoAnterior[MAX_NOS]);

int
determinarModoOperacaoTrans(elemento netlistLido[MAX_ELEMENTOS], int * , double solucaoAnterior[MAX_NOS], int);

int 
zeraSistema_PO(double sistema[MAX_NOS+1][MAX_NOS+2], int * );

int
zeraSistema_JW(cppcomplex sistema[MAX_NOS+1][MAX_NOS+2], int *);

int 
printaSNM_PO(double sistema[MAX_NOS+1][MAX_NOS+2], int *);

int 
resolverSNM_PO(double sistema[MAX_NOS+1][MAX_NOS+2], int *);

int
inicializacaoAleatoria(double solucaoAnterior[MAX_NOS], int * ,int *, double erros[MAX_NOS+1]);

int
salvaUltimaIteracao(double Yn[MAX_NOS+1][MAX_NOS+2], double solucaoAnterior[MAX_NOS], int *);

int 
testeDeConvergencia(double Yn[MAX_NOS+1][MAX_NOS+2],  double solucaoAnteior[MAX_NOS], int *, double erros[MAX_NOS+1],  int);

int
newtonRaphson_PO(double Yn[MAX_NOS+1][MAX_NOS+2], double solucaoAnterior[MAX_NOS] , elemento netlistLido[MAX_ELEMENTOS],
int * , int * , int *, int, int vetorIndicesTrans[MAX_ELEMENTOS]);

int
montaSNM_JW (double ,cppcomplex Yn_JW[MAX_NOS+1][MAX_NOS+2], elemento netlistLido[MAX_ELEMENTOS], 
int * , int * , int * , double solucaoAnterior[MAX_NOS]);

int 
resolverSNM_JW (cppcomplex sistema[MAX_NOS+1][MAX_NOS+2], int *);

int
printaSNM_JW(cppcomplex sistema[MAX_NOS+1][MAX_NOS+2], int *);

int
printaEstampa(int indice, elemento netlist[MAX_ELEMENTOS], int numVariaveis, double Yn[MAX_NOS+1][MAX_NOS+2]);

int
calculaCapacitancias(elemento netlist[MAX_ELEMENTOS], int indicesTrans[MAX_ELEMENTOS], int numTransistores);

int
escreverTabelaParametrosTransistor(elemento netlist[MAX_ELEMENTOS], int indicesTrans[MAX_ELEMENTOS], int numTransistores,  char * nomeArquivo);

double
moduloComplexo (cppcomplex );

int
quantidadeTrans(elemento netlist[MAX_ELEMENTOS] , int , int vetorIndicesTrans[MAX_ELEMENTOS]);

// definicao das  funcoes - colocadas na mesma ordem que os propotios

//-----------------------------------------------

int
lerNetlist(char listaDeNos[MAX_NOS + 1][MAX_NOME + 2], char *nomeArquivo, int * pNumElementos, int * pNumVariaveis, int * pNumNos, elemento netlistLido[MAX_ELEMENTOS], 
parametrosAnaliseAC * pAnaliseAC, int * pPontoOperacao)
{
	FILE *arquivo;
	char txt[MAX_LINHA + 1];
	char tipoDoElemento;
	char *posicaoleituraParametro;
	int codigoDeErro;
	int indiceNetlistA = 0;
	int indiceNetlistB = 0;
	int numVariaveis = 0;
	int numNos = 0;
	int numElementos = 0 ;
	int verificaElementoReativo = 0;
	int qtdTransistores = 0;
	char noA[MAX_NOME];
	char noB[MAX_NOME];
	char noC[MAX_NOME];
	char noD[MAX_NOME];
	char noS[MAX_NOME];
	char noG[MAX_NOME];
	char indutorA[MAX_NOME];
	char indutorB[MAX_NOME];
	char tipoTrans[MAX_TIPO_TRANS];
	double acoplamento = 0.0;
	
	int i;
	
	arquivo = fopen(nomeArquivo,"r");
	if (arquivo == 0) 
	{
		printf("Arquivo inexistente.\n");
		return 2;
	}

	printf("Lendo netlist:"); 
	fgets(txt, MAX_LINHA, arquivo);
	

	strcpy(listaDeNos[0],"0"); 

	qtdTransistores = 0;

	while (fgets(txt, MAX_LINHA, arquivo) ) 
	{
		numElementos++; /* Nao usa o netlistLido[0] */
		if (numElementos > MAX_ELEMENTOS)
		{
			printf("O programa so aceita ate %d elementos\n",MAX_ELEMENTOS);
		}
		txt[0]=toupper(txt[0]); /* Se a pessoa colocar "r" no nome do resistor, vira R */
		tipoDoElemento = txt[0]; /* Identifico o tipo do elemento que será lido */
		sscanf(txt,"%10s", netlistLido[numElementos].nome); /*Faco a leitura do nome do elemento, que tem que ter no maximo 10 caracters + EOS */
		posicaoleituraParametro = txt + strlen(netlistLido[numElementos].nome); /* Levo o ponteiro pro começo do parâmetro*/

		if (tipoDoElemento =='R')
		{
			sscanf(posicaoleituraParametro ,"%10s%10s%lg", noA, noB, &netlistLido[numElementos].valor);
			printf("%s %s %s %g\n",netlistLido[numElementos].nome, noA, noB, netlistLido[numElementos].valor);
			netlistLido[numElementos].a= controleDosNos(noA, listaDeNos, &numVariaveis);
			netlistLido[numElementos].b= controleDosNos(noB, listaDeNos, &numVariaveis);
		}
		else if (tipoDoElemento == 'L' || tipoDoElemento == 'C') 
		{
			verificaElementoReativo = TEM_REATIVO;
			sscanf(posicaoleituraParametro ,"%10s%10s%lg", noA, noB, &netlistLido[numElementos].valor);
			printf("%s %s %s %g\n",netlistLido[numElementos].nome, noA, noB, netlistLido[numElementos].valor);
			netlistLido[numElementos].a= controleDosNos(noA, listaDeNos, &numVariaveis);
			netlistLido[numElementos].b= controleDosNos(noB, listaDeNos, &numVariaveis);
			netlistLido[numElementos].m=0;
		}
		else if (tipoDoElemento =='I' || tipoDoElemento =='V')
		{ 
			double modAuxiliar;
			double a,b, anguloGraus;
			cppcomplex valorACAux(0.0,0.0);
			
			sscanf(posicaoleituraParametro ,"%10s%10s%lg%lg%lg", noA, noB, &modAuxiliar, &netlistLido[numElementos].fase, &netlistLido[numElementos].valor);
			printf("%s %s %s %g %g %g\n",netlistLido[numElementos].nome, noA, noB, modAuxiliar, netlistLido[numElementos].fase, netlistLido[numElementos].valor);
			anguloGraus =  (netlistLido[numElementos].fase);
			a = modAuxiliar * cos(anguloGraus*PI/180.0);
			b = modAuxiliar * sin(anguloGraus*PI/180.0);
			valorACAux = a + b*I;
			/*
			modAuxiliar = cabs(valorACAux);
			printf("EITA %g\n", modAuxiliar);
			*/
			netlistLido[numElementos].a= controleDosNos(noA, listaDeNos, &numVariaveis);
			netlistLido[numElementos].b= controleDosNos(noB, listaDeNos, &numVariaveis);
			netlistLido[numElementos].valorAC= valorACAux;
		}
		
		else if (tipoDoElemento =='G' || tipoDoElemento =='E' || tipoDoElemento =='F' || tipoDoElemento =='H') 
		{
			sscanf(posicaoleituraParametro ,"%10s%10s%10s%10s%lg", noA, noB, noC, noD, &netlistLido[numElementos].valor);
			printf("%s %s %s %s %s %g\n",netlistLido[numElementos].nome, noA, noB, noC, noD,netlistLido[numElementos].valor);
			netlistLido[numElementos].a=controleDosNos(noA, listaDeNos, &numVariaveis);
			netlistLido[numElementos].b=controleDosNos(noB, listaDeNos, &numVariaveis);
			netlistLido[numElementos].c=controleDosNos(noC, listaDeNos, &numVariaveis);
			netlistLido[numElementos].d=controleDosNos(noD, listaDeNos, &numVariaveis);  
		}
		else if (tipoDoElemento =='O') 
		{
			sscanf(posicaoleituraParametro ,"%10s%10s%10s%10s", noA, noB, noC, noD);
			printf("%s %s %s %s %s\n",netlistLido[numElementos].nome, noA, noB, noC, noD);
			netlistLido[numElementos].a=controleDosNos(noA, listaDeNos, &numVariaveis);
			netlistLido[numElementos].b=controleDosNos(noB, listaDeNos, &numVariaveis);
			netlistLido[numElementos].c=controleDosNos(noC, listaDeNos, &numVariaveis);;
			netlistLido[numElementos].d=controleDosNos(noD, listaDeNos, &numVariaveis);
		}
		else if (tipoDoElemento =='K') 
		{
			verificaElementoReativo = TEM_REATIVO;
			sscanf(posicaoleituraParametro ,"%10s%10s%lf", indutorA, indutorB, &acoplamento);
			codigoDeErro = acharElemento(indutorA, &indiceNetlistA, netlistLido,  numElementos);
			if (codigoDeErro != 0)
			{
				return codigoDeErro;
			}
			codigoDeErro = acharElemento(indutorB, &indiceNetlistB, netlistLido,  numElementos);
			if (codigoDeErro != 0)
			{
				return codigoDeErro;
			}
			netlistLido[indiceNetlistA].m =  acoplamento * sqrt((netlistLido[indiceNetlistA].valor) * (netlistLido[indiceNetlistB].valor));
			netlistLido[indiceNetlistB].m =  acoplamento * sqrt((netlistLido[indiceNetlistA].valor) * (netlistLido[indiceNetlistB].valor));
			netlistLido[indiceNetlistA].par = indiceNetlistB;
			netlistLido[indiceNetlistB].par = indiceNetlistA;	 
			numElementos--;
		}
		else if (tipoDoElemento == 'M') /* Transistor */
		{   	
			double comprimentoTransistorTeste, larguraTransistorTeste, kTransistor, vtzeroTransistor, lambdaTransistor, gamaTransistor, phiTransistor,ldTransistor;
			
			sscanf(posicaoleituraParametro,"%10s%10s%10s%10s%4s L=%lg W=%lg%lg%lg%lg%lg%lg%lg", noD, noG, noS, noB, tipoTrans, &comprimentoTransistorTeste, 
			&larguraTransistorTeste, &kTransistor, &vtzeroTransistor, &lambdaTransistor, &gamaTransistor, &phiTransistor, &ldTransistor);
			
			
			
			if (( strcmp(tipoTrans, "PMOS") != 0) && (strcmp(tipoTrans, "NMOS") != 0))
			{
				printf("Tipo transistor errado \n");    		
				printf("Tipos aceitos: NMOS ou PMOS");
				return(TIPO_TRANS_INVALIDO);
			}
			
			printf("%s %s %s %s %s %s %lg %lg %lg %lg %lg %lg %lg %lg\n",netlistLido[numElementos].nome, noD, noG, noS, noB, tipoTrans, 
			comprimentoTransistorTeste, larguraTransistorTeste, kTransistor, vtzeroTransistor, lambdaTransistor, gamaTransistor, phiTransistor, ldTransistor);
			netlistLido[numElementos].d=controleDosNos(noD, listaDeNos, &numVariaveis);
			netlistLido[numElementos].g=controleDosNos(noG, listaDeNos, &numVariaveis);
			netlistLido[numElementos].s=controleDosNos(noS, listaDeNos, &numVariaveis);
			netlistLido[numElementos].b=controleDosNos(noB, listaDeNos, &numVariaveis);
			strcpy(netlistLido[numElementos].tipoTrans, tipoTrans);
			netlistLido[numElementos].comprimentoTrans = comprimentoTransistorTeste;
			netlistLido[numElementos].larguraTrans = larguraTransistorTeste;
			netlistLido[numElementos].kTrans = kTransistor;
			netlistLido[numElementos].vtzeroTrans = vtzeroTransistor;
			netlistLido[numElementos].lambdaTrans = lambdaTransistor;
			netlistLido[numElementos].gamaTrans = gamaTransistor;
			netlistLido[numElementos].phiTrans = phiTransistor;
			netlistLido[numElementos].ldTrans = ldTransistor; 
			netlistLido[numElementos].gmGS=0.0;
			netlistLido[numElementos].gmDS=0.0;
			netlistLido[numElementos].gmBS=0.0;
			netlistLido[numElementos].correnteDCmos=0.0;
			netlistLido[numElementos].inverteu=0;
			netlistLido[numElementos].Cgs=0.0;
			netlistLido[numElementos].Cgd=0.0;
			netlistLido[numElementos].Cbg=0.0;
			
			
			*pPontoOperacao = 0;
			


		}
		else if (tipoDoElemento =='*')  /* Comentario comeca com "*" */
		{
			printf("Comentario: %s\n",txt);
			numElementos--;
		}	  
		else if (tipoDoElemento =='.')  /* Linha comandos AC ".AC" */
		{ 
			char escala [3];
			double comeco, fim, numPontos; /* funcao sscanf so consegue passar numero por referencia*/
			parametrosAnaliseAC auxiliarAnaliseAC;
			
			if (txt[1] != 'A')
			{
				printf("Elemento desconhecido: %s\n",txt);
				getch();
				return(ELEMENTO_DESCONHECIDO);
			}
			if (txt[2] != 'C')
			{
				printf("Elemento desconhecido: %s\n",txt);
				getch();
				return(ELEMENTO_DESCONHECIDO);
			}

			posicaoleituraParametro = txt + 3; /*ta no ponto '.' tem que andar 'a' 'c' ' '*/
			sscanf(posicaoleituraParametro, "%3s%lg%lg%lg", escala, &numPontos, &comeco, &fim);
			auxiliarAnaliseAC.numPontos = numPontos;
			printf("%s %.2lf %.2lf %.2lf\n", escala, numPontos, comeco, fim);
			
			escala[0] = toupper(escala[0]);
			escala[1] = toupper(escala[1]);
			escala[2] = toupper(escala[2]);
			
			if ((strcmp(escala, "LIN") != 0) && (strcmp(escala, "OCT") != 0) && (strcmp(escala, "DEC") != 0))
			{
				printf("Tipo escala errado \n");    		
				printf("Tipos aceitos: LIN ou OCT ou DEC");
				return(TIPO_ESCALA_INVALIDO);
			}
			
			if (strcmp(escala, "LIN") == 0)
			auxiliarAnaliseAC.escala = LIN;
			if (strcmp(escala, "OCT") == 0)
			auxiliarAnaliseAC.escala = OCT;
			if (strcmp(escala, "DEC") == 0)
			auxiliarAnaliseAC.escala = DEC;
			
			/* alocando os outros parametros da funcao sscanf */  
			auxiliarAnaliseAC.comeco = comeco;
			auxiliarAnaliseAC.fim = fim;
			
			/* passagem por referencia dos parametros*/
			*pAnaliseAC = auxiliarAnaliseAC;
			
			numElementos--;
		}
		
		else
		{
			printf("Elemento desconhecido: %s\n",txt);
			getch();
			return(ELEMENTO_DESCONHECIDO);
		}
	}
	fclose(arquivo);

	/* Acrescenta variaveis de corrente acima dos nos, anotando no netlist */
	numNos = numVariaveis;

	for (i = 1; i <= numElementos; i++)
	{
		tipoDoElemento = netlistLido[i].nome[0];
		if (tipoDoElemento =='V' || tipoDoElemento =='E' || tipoDoElemento =='F' || tipoDoElemento =='O' || tipoDoElemento =='L')
		{
			numVariaveis++;
			if (numVariaveis > MAX_NOS) 
			{
				printf("As correntes extra excederam o numero de variaveis permitido (%d)\n", MAX_NOS);
				return (NUMERO_MAX_CORRENTES);
			}
			strcpy(listaDeNos[numVariaveis],"j");  
			strcat(listaDeNos[numVariaveis], netlistLido[i].nome);
			netlistLido[i].x = numVariaveis;
		}
		else if (tipoDoElemento =='H') 
		{
			numVariaveis = numVariaveis + 2;
			if (numVariaveis > MAX_NOS) 
			{
				printf("As correntes extra excederam o numero de variaveis permitido (%d)\n",MAX_NOS);
				return (NUMERO_MAX_CORRENTES);
			}
			strcpy(listaDeNos[numVariaveis - 1],"jx"); strcat(listaDeNos[numVariaveis - 1],netlistLido[i].nome);
			netlistLido[i].x= numVariaveis - 1;
			strcpy(listaDeNos[numVariaveis],"jy"); strcat(listaDeNos[numVariaveis],netlistLido[i].nome);
			netlistLido[i].y=numVariaveis;
		}
	}
	
	codigoDeErro = printaNetlistFinalizado(listaDeNos, netlistLido, numVariaveis, numElementos);
	
	/* passagem por referencia*/
	*pNumNos = numNos;
	*pNumVariaveis = numVariaveis;
	*pNumElementos = numElementos;

	
	

	return OK;
}


//---------------------------------------------------------------------------

int escreverTabela (char listaDeNos[MAX_NOS + 1][MAX_NOME + 2], int linha, int numPontos, cppcomplex sistema[MAX_NOS+1][MAX_NOS+2], int *pNumVariaveis, double * pFrequenciaAngular, char * nomeArquivo, elemento netlistLido[MAX_ELEMENTOS])
{
	int numVariaveis;
	int contador;
	double frequenciaAngular;
	FILE *arquivo;
	char *nomeArquivoTab;
	char nome [MAX_NOME+1];
	char tipoDoElemento;
	
	/*inicializar variaveis*/
	numVariaveis = *pNumVariaveis;
	nomeArquivoTab = strtok(nomeArquivo,".");
	strcat (nomeArquivoTab,".tab");
	frequenciaAngular = *pFrequenciaAngular;
	
	if (linha == 0) /*primeira linha, nomes das variaveis */
	{ 
		if (!(arquivo = fopen (nomeArquivoTab, "w")))
		{
			printf("Erro ao abrir o arquivo #%i: %s\n", errno, strerror(errno));
			return ERRO_ABRINDO_ARQUIVO;
		} 
		fprintf (arquivo, "f ");
		for (contador = 1; contador <= numVariaveis; contador++)
		fprintf(arquivo, "%sm %sf ", listaDeNos[contador],listaDeNos[contador]);
		fprintf (arquivo, "\n");
	}
	else /*não for a primeira linha*/
	{
		if (!(arquivo = fopen (nomeArquivoTab, "a")))
		{
			printf("Erro ao abrir o arquivo #%i: %s\n", errno, strerror(errno));
			return ERRO_ABRINDO_ARQUIVO;
		}
	}
	
	/* primeira coluna*/
	fprintf (arquivo, "%lg ", frequenciaAngular/(2*PI));
	for (contador = 1; contador < numVariaveis; contador++) /*modulo e e fase*/
	if (sistema[contador][numVariaveis+1] != (0.0,0.0))
	{
		fprintf (arquivo,"%lg %lg ", moduloComplexo (sistema[contador][numVariaveis+1]), std::arg (sistema[contador][numVariaveis+1])*180/PI);
	}
	else
	{
		fprintf (arquivo,"%lg %lg ", 0.0, 0.0);
	}
	if (sistema[contador][numVariaveis+1] != (0.0,0.0))
	{
		fprintf (arquivo,"%lg %lg\n", moduloComplexo(sistema[contador][numVariaveis+1]), std::arg (sistema[contador][numVariaveis+1])*180/PI); /*ultima linha*/
	}
	else
	{
		fprintf (arquivo,"%lg %lg\n", 0.0, 0.0);
	}
	
	fclose(arquivo);		
	return 0;
}

//---------------------------------------------------------------------------

int
escreverTabelaPO (char listaDeNos[MAX_NOS + 1][MAX_NOME + 2], double sistema[MAX_NOS+1][MAX_NOS+2], int *pNumVariaveis,  char * nomeArquivo, elemento netlistLido[MAX_ELEMENTOS])
{
	int numVariaveis;
	int contador;
	FILE *arquivo;
	char *nomeArquivoTabPO;
	char nome [MAX_NOME+1];
	char tipoDoElemento;
	
	/*inicializar variaveis*/
	numVariaveis = *pNumVariaveis;
	nomeArquivoTabPO = strtok(nomeArquivo,".");
	strcat (nomeArquivoTabPO,"_PO.tab");

	if (!(arquivo = fopen (nomeArquivoTabPO, "w")))
	{
		printf("Erro ao abrir o arquivo #%i: %s\n", errno, strerror(errno));
		return ERRO_ABRINDO_ARQUIVO;
	} 
	
	fprintf (arquivo, "PO \n");
	for (contador = 1; contador <= numVariaveis; contador++)
	{
		fprintf(arquivo, "%s %lg\n", listaDeNos[contador], sistema[contador][numVariaveis+1]);
	}
	fclose(arquivo);		
	return 0;
}

//---------------------------------------------------------------------------

int 
controleDosNos(char *nomeNo, char listaDeNos[MAX_NOS + 1][MAX_NOME + 2], int* qtdVariaveis)
{
	int i = 0;
	int achou = 0;
	int qtd = 0;
	
	qtd = *qtdVariaveis;
	i=0; 
	achou=0;
	
	while (!achou && i<= qtd)
	{
		if (!(achou =! strcmp(nomeNo, listaDeNos[i])))
		i++;
	}
	if (!achou) 
	{
		if (qtd == MAX_NOS) 
		{
			printf("O programa so aceita ate %d nos\n", qtd);
			exit(NUMERO_MAX_NOS);
		}   
		qtd++;
		*qtdVariaveis = qtd;
		strcpy(listaDeNos[qtd], nomeNo);
		return qtd; /* novo no */
	}
	else
	{
		return i; /* no ja conhecido */
	}
}


//---------------------------------------------------------------------------


int
printaNetlistFinalizado(char listaDeNos[MAX_NOS + 1][MAX_NOME + 2], elemento netlist[MAX_ELEMENTOS], int numVariaveis, int numElementos)
{
	int i = 0;
	char tipoDoElemento = 0;

	printf("Variaveis internas: \n");
	for (i=1; i <= numVariaveis;)
	{
		printf(" #%d %s\n", i, listaDeNos[i]);
		i++;
	}
	getch();
	printf("Netlist interno final\n");
	for (i=1; i<=numElementos;) 
	{
		tipoDoElemento = netlist[i].nome[0];
		if (tipoDoElemento=='R' || tipoDoElemento=='L' || tipoDoElemento=='C' )
		{
			printf("%s %d %d %g\n", netlist[i].nome, netlist[i].a, netlist[i].b, netlist[i].valor);
		}
		else if (tipoDoElemento=='I' || tipoDoElemento=='V' )
		{
			double modulo;
			modulo = moduloComplexo(netlist[i].valorAC);
			printf ("%s %d %d %g %g %g\n",netlist[i].nome, netlist[i].a, netlist[i].b, modulo, netlist[i].fase, netlist[i].valor);
		}  
		else if (tipoDoElemento=='G' || tipoDoElemento=='E' || tipoDoElemento=='F' || tipoDoElemento=='H') 
		{
			printf("%s %d %d %d %d %g\n",netlist[i].nome,netlist[i].a,netlist[i].b,netlist[i].c,netlist[i].d,netlist[i].valor);
		}
		else if (tipoDoElemento=='M') 
		{
			printf("%s %d %d %d %d\n",netlist[i].nome,netlist[i].d,netlist[i].g,netlist[i].s,netlist[i].b);
		}
		i++;
	}  
	return OK;
}

//---------------------------------------------------------------------------

int
acharElemento (char*nomeElemento, int*indiceNetlist, elemento netlist[MAX_ELEMENTOS], int numElementos)
{
	int i = 0;
	int achou = 0;


	i=1;
	while ((i <= numElementos) && (achou == 0))
	{
		if (strcmp (nomeElemento,netlist[i].nome) == 0)
		{
			*indiceNetlist = i;
			achou = 1;
		}		
		i++;
	}  
	return 0;
}

//---------------------------------------------------------------------------


int
montaSNM_PO(double Yn[MAX_NOS+1][MAX_NOS+2], elemento netlistLido[MAX_ELEMENTOS], int * pNumElementos, int * pNumVariaveis, int * pNumNos,
double solucaoAnterior[MAX_NOS])
{
	char tipoDoElemento;
	int codigoDeErro; 
	int numVariaveis;
	int numNos;
	int numElementos;	
	int modoOperacao;
	double	g;
	double constanteMOS;
	double iD, vBSnovo, aux, gmDS, gmGS, gmBS, correnteDCmos,vtNovo,vt,vGS,vDS,vBS;
	char tipoTrans[MAX_TIPO_TRANS];

	
	/* contadores */
	int i = 0;
	
	/*inicializacao*/
	modoOperacao = 0;
	numNos = *pNumNos;
	numVariaveis = *pNumVariaveis;
	numElementos = *pNumElementos; 

	

	/* Monta estampas para o calculo do ponto de operacao*/
	for (i=1; i<=numElementos; i++) 
	{
		tipoDoElemento = netlistLido[i].nome[0];   
		if (tipoDoElemento=='R') /* resistor */ 
		{ 
			g=1/netlistLido[i].valor;
			Yn[netlistLido[i].a][netlistLido[i].a]+=g;
			Yn[netlistLido[i].b][netlistLido[i].b]+=g;
			Yn[netlistLido[i].a][netlistLido[i].b]-=g;
			Yn[netlistLido[i].b][netlistLido[i].a]-=g;
		}
		
		else if (tipoDoElemento=='G') /* fonte corrente controlada tensao */
		{
			g=netlistLido[i].valor;
			Yn[netlistLido[i].a][netlistLido[i].c]+=g;
			Yn[netlistLido[i].b][netlistLido[i].d]+=g;
			Yn[netlistLido[i].a][netlistLido[i].d]-=g;
			Yn[netlistLido[i].b][netlistLido[i].c]-=g;
		}
		else if (tipoDoElemento=='I') /* fonte corrente */
		{
			g=netlistLido[i].valor;
			Yn[netlistLido[i].a][numVariaveis+1]-=g;
			Yn[netlistLido[i].b][numVariaveis+1]+=g;
		}
		else if (tipoDoElemento=='V') /* fonte tensao */
		{
			Yn[netlistLido[i].a][netlistLido[i].x]+=1;
			Yn[netlistLido[i].b][netlistLido[i].x]-=1;
			Yn[netlistLido[i].x][netlistLido[i].a]-=1;
			Yn[netlistLido[i].x][netlistLido[i].b]+=1;
			Yn[netlistLido[i].x][numVariaveis+1]-=netlistLido[i].valor;
		}
		else if (tipoDoElemento=='E') /* fonte tensao controlada tensao */
		{
			g=netlistLido[i].valor;
			Yn[netlistLido[i].a][netlistLido[i].x]+=1;
			Yn[netlistLido[i].b][netlistLido[i].x]-=1;
			Yn[netlistLido[i].x][netlistLido[i].a]-=1;
			Yn[netlistLido[i].x][netlistLido[i].b]+=1;
			Yn[netlistLido[i].x][netlistLido[i].c]+=g;
			Yn[netlistLido[i].x][netlistLido[i].d]-=g;
		}
		else if (tipoDoElemento=='F') /* fonte corrente controlada corrente */
		{
			g=netlistLido[i].valor;
			Yn[netlistLido[i].a][netlistLido[i].x]+=g;
			Yn[netlistLido[i].b][netlistLido[i].x]-=g;
			Yn[netlistLido[i].c][netlistLido[i].x]+=1;
			Yn[netlistLido[i].d][netlistLido[i].x]-=1;
			Yn[netlistLido[i].x][netlistLido[i].c]-=1;
			Yn[netlistLido[i].x][netlistLido[i].d]+=1;
		}
		else if (tipoDoElemento=='H') /* fonte tensao controlada corrente */ 
		{
			g=netlistLido[i].valor;
			Yn[netlistLido[i].a][netlistLido[i].y]+=1;
			Yn[netlistLido[i].b][netlistLido[i].y]-=1;
			Yn[netlistLido[i].c][netlistLido[i].x]+=1;
			Yn[netlistLido[i].d][netlistLido[i].x]-=1;
			Yn[netlistLido[i].y][netlistLido[i].a]-=1;
			Yn[netlistLido[i].y][netlistLido[i].b]+=1;
			Yn[netlistLido[i].x][netlistLido[i].c]-=1;
			Yn[netlistLido[i].x][netlistLido[i].d]+=1;
			Yn[netlistLido[i].y][netlistLido[i].x]+=g;
		}
		else if (tipoDoElemento=='O') 
		{
			Yn[netlistLido[i].a][netlistLido[i].x]+=1;
			Yn[netlistLido[i].b][netlistLido[i].x]-=1;
			Yn[netlistLido[i].x][netlistLido[i].c]+=1;
			Yn[netlistLido[i].x][netlistLido[i].d]-=1;
		}
		else if (tipoDoElemento=='L') /* indutor */
		{
			g = RESISTENCIA_CURTO;
			Yn[netlistLido[i].a][netlistLido[i].x]+=1;
			Yn[netlistLido[i].b][netlistLido[i].x]-=1;
			Yn[netlistLido[i].x][netlistLido[i].a]-=1;
			Yn[netlistLido[i].x][netlistLido[i].b]+=1;
			Yn[netlistLido[i].x][netlistLido[i].x]+=g;
		}
		else if (tipoDoElemento=='C') /* capacitor */
		{
			g = 1/RESISTENCIA_ABERTO;
			Yn[netlistLido[i].a][netlistLido[i].a] += g;
			Yn[netlistLido[i].b][netlistLido[i].b] += g;
			Yn[netlistLido[i].a][netlistLido[i].b] -= g;
			Yn[netlistLido[i].b][netlistLido[i].a] -= g;
		}
		else if (tipoDoElemento == 'M') /*  transistor */
		{ 	
			g=1/RESISTENCIA_ABERTO;
			//g = 0;
			constanteMOS = netlistLido[i].kTrans * netlistLido[i].larguraTrans/ netlistLido[i].comprimentoTrans;
			
			strncpy (tipoTrans, netlistLido[i].tipoTrans,(size_t) MAX_TIPO_TRANS);
			tipoTrans[MAX_TIPO_TRANS] = '\0';
			strncpy (netlistLido[i].tipoTrans,tipoTrans,(size_t) MAX_TIPO_TRANS);
			netlistLido[i].tipoTrans[MAX_TIPO_TRANS] = '\0';	 
			
			if(netlistLido[i].tipoTrans[0] == 'N')
			{
				if (solucaoAnterior[netlistLido[i].d] < solucaoAnterior[netlistLido[i].s]) 
				{ 
					aux=netlistLido[i].d;
					netlistLido[i].d=netlistLido[i].s;
					netlistLido[i].s=aux;
					netlistLido[i].inverteu += 1;
				}
				vGS = solucaoAnterior[netlistLido[i].g]-solucaoAnterior[netlistLido[i].s];
				vDS =  solucaoAnterior[netlistLido[i].d]-solucaoAnterior[netlistLido[i].s];
				vBS =  solucaoAnterior[netlistLido[i].b]-solucaoAnterior[netlistLido[i].s];
				vBSnovo = (vBS>netlistLido[i].phiTrans/2)? (netlistLido[i].phiTrans/2):vBS; 
				vt = netlistLido[i].vtzeroTrans + netlistLido[i].gamaTrans*(sqrt(netlistLido[i].phiTrans - vBSnovo) - sqrt(netlistLido[i].phiTrans));
				netlistLido[i].vt = vt;

				if (vGS<vt) /* corte */
				{
					/*	printf("%s ENTROU NO CORTE NMOS \n\n", netlistLido[i].nome); */
					netlistLido[i].modoOperacao = 0;
					netlistLido[i].gmGS = 0.0;
					netlistLido[i].gmDS = 0.0;
					netlistLido[i].gmBS = 0.0;
					netlistLido[i].correnteDCmos = 0.0;
				}
				else 
				{
					if (vDS <= (vGS-vt))/* triodo */ 
					{				
						/*	printf("%s ENTROU NO TRIODO NMOS \n\n",netlistLido[i].nome); */	
						netlistLido[i].modoOperacao = 1; 
						iD = constanteMOS*(2*(vGS-vt)*vDS - vDS*vDS)*(1+netlistLido[i].lambdaTrans*vDS);
						gmGS = constanteMOS*2*vDS*(1+ netlistLido[i].lambdaTrans * vDS); 
						gmDS = constanteMOS*( 2*(vGS-vt) - 2*vDS+ 4*netlistLido[i].lambdaTrans*(vGS-vt)*vDS - 3*netlistLido[i].lambdaTrans*vDS*vDS);	
					}
					else /*saturacao*/
					{
						/*	printf(" %s ENTROU NA SATURACAO NMOS \n\n", netlistLido[i].nome); */
						netlistLido[i].modoOperacao = 2;
						iD = constanteMOS*(vGS-vt)*(vGS-vt)*(1+netlistLido[i].lambdaTrans*vDS);
						gmGS = 2*constanteMOS*(vGS-vt)*(1+netlistLido[i].lambdaTrans*vDS);
						gmDS = constanteMOS*(vGS-vt)*(vGS-vt)*netlistLido[i].lambdaTrans;	 
					}
					gmBS = (gmGS * netlistLido[i].gamaTrans)/(2*sqrt(netlistLido[i].phiTrans - vBSnovo));
					correnteDCmos = iD - gmGS*vGS - gmDS*vDS - gmBS*vBSnovo;	
					netlistLido[i].gmDS = gmDS;
					netlistLido[i].gmGS = gmGS;
					netlistLido[i].gmBS = gmBS;
					netlistLido[i].correnteDCmos = correnteDCmos;
					
					/* montando as estampas do NPN*/
					Yn[netlistLido[i].d][netlistLido[i].d]+=gmDS;
					Yn[netlistLido[i].s][netlistLido[i].s]+=gmDS;
					Yn[netlistLido[i].d][netlistLido[i].s]-=gmDS;
					Yn[netlistLido[i].s][netlistLido[i].d]-=gmDS;
					
					Yn[netlistLido[i].d][netlistLido[i].b]+=gmBS;
					Yn[netlistLido[i].s][netlistLido[i].s]+=gmBS;
					Yn[netlistLido[i].d][netlistLido[i].s]-=gmBS;
					Yn[netlistLido[i].s][netlistLido[i].b]-=gmBS;
					
					Yn[netlistLido[i].d][netlistLido[i].g]+=gmGS;
					Yn[netlistLido[i].s][netlistLido[i].s]+=gmGS;
					Yn[netlistLido[i].d][netlistLido[i].s]-=gmGS;
					Yn[netlistLido[i].s][netlistLido[i].g]-=gmGS;
					
					Yn[netlistLido[i].d][numVariaveis + 1]-=correnteDCmos;
					Yn[netlistLido[i].s][numVariaveis + 1]+=correnteDCmos;

				}      
				netlistLido[i].vDS = vDS;
				netlistLido[i].vGS = vGS;
				netlistLido[i].vBS = vBSnovo;  
				
			}
			if(netlistLido[i].tipoTrans[0] == 'P')
			{
				if (solucaoAnterior[netlistLido[i].d] > solucaoAnterior[netlistLido[i].s]) 
				{ 
					aux=netlistLido[i].d;
					netlistLido[i].d=netlistLido[i].s;
					netlistLido[i].s=aux;
					netlistLido[i].inverteu += 1;
				}
				vGS = solucaoAnterior[netlistLido[i].g]-solucaoAnterior[netlistLido[i].s];
				vDS =  solucaoAnterior[netlistLido[i].d]-solucaoAnterior[netlistLido[i].s];
				vBS =  solucaoAnterior[netlistLido[i].b]-solucaoAnterior[netlistLido[i].s];
				vBSnovo = ((-vBS)>netlistLido[i].phiTrans/2)? (-netlistLido[i].phiTrans/2):vBS;
				vt = netlistLido[i].vtzeroTrans + netlistLido[i].gamaTrans*(sqrt(netlistLido[i].phiTrans - vBSnovo) - sqrt(netlistLido[i].phiTrans));
				vtNovo = -vt;
				netlistLido[i].vt = vtNovo;
				if ( vGS > vtNovo) /* corte */
				{
					/*	printf(" %s  ENTROU NO CORTE PMOS \n\n", netlistLido[i].nome);*/
					netlistLido[i].modoOperacao = 0;
					netlistLido[i].gmGS = 0.0;
					netlistLido[i].gmDS = 0.0;
					netlistLido[i].gmBS = 0.0;
					netlistLido[i].correnteDCmos = 0.0;
				}
				else
				{
					if (vDS > (vGS-vtNovo))/* triodo */ 
					{
						/*	printf("%s ENTROU NO TRIODO PMOS \n\n", netlistLido[i].nome);	*/
						netlistLido[i].modoOperacao = 1; 
						iD = constanteMOS*(2*(-vGS+vtNovo)*(-vDS) - vDS*vDS)*(1+netlistLido[i].lambdaTrans*(-vDS));
						gmGS = constanteMOS*2*(-vDS)*(1+ netlistLido[i].lambdaTrans * (-vDS)); 
						gmDS = constanteMOS*(2*(-vGS+vtNovo) - 2*(-vDS)+ 4*netlistLido[i].lambdaTrans*(-vGS+vtNovo)*(-vDS) - 3*netlistLido[i].lambdaTrans*vDS*vDS);	
						
					}
					else /* saturacao*/
					{
						/*	printf("%s ENTROU NA SATURACAO NMOS \n\n", netlistLido[i].nome);*/
						netlistLido[i].modoOperacao = 2;
						iD = constanteMOS*(-vGS+vtNovo)*(-vGS+vtNovo)*(1+netlistLido[i].lambdaTrans*(-vDS));
						gmGS = 2*constanteMOS*(-vGS+vtNovo)*(1+netlistLido[i].lambdaTrans*(-vDS));
						gmDS = constanteMOS*(-vGS+vtNovo)*(-vGS+vtNovo)*netlistLido[i].lambdaTrans;	 
					}
					gmBS = (gmGS * netlistLido[i].gamaTrans)/(2*sqrt(netlistLido[i].phiTrans - vBSnovo));
					correnteDCmos = iD + gmGS*vGS + gmDS*vDS + gmBS*vBSnovo;
					
					netlistLido[i].gmDS = gmDS;
					netlistLido[i].gmGS = gmGS;
					netlistLido[i].gmBS = gmBS;
					netlistLido[i].correnteDCmos = correnteDCmos;

					
					/* montando as estampas do PNP*/
					Yn[netlistLido[i].d][netlistLido[i].d]+=gmDS;
					Yn[netlistLido[i].s][netlistLido[i].s]+=gmDS;
					Yn[netlistLido[i].d][netlistLido[i].s]-=gmDS;
					Yn[netlistLido[i].s][netlistLido[i].d]-=gmDS;
					
					Yn[netlistLido[i].s][netlistLido[i].s]+=gmBS;
					Yn[netlistLido[i].d][netlistLido[i].b]+=gmBS;
					Yn[netlistLido[i].s][netlistLido[i].b]-=gmBS;
					Yn[netlistLido[i].d][netlistLido[i].s]-=gmBS;
					
					Yn[netlistLido[i].s][netlistLido[i].s]+=gmGS;
					Yn[netlistLido[i].d][netlistLido[i].g]+=gmGS;
					Yn[netlistLido[i].s][netlistLido[i].g]-=gmGS;
					Yn[netlistLido[i].d][netlistLido[i].s]-=gmGS;
					
					Yn[netlistLido[i].d][numVariaveis + 1]+=correnteDCmos;
					Yn[netlistLido[i].s][numVariaveis + 1]-=correnteDCmos;   	
					
				}	
				netlistLido[i].vDS = vDS;
				netlistLido[i].vGS = vGS;
				netlistLido[i].vBS = vBSnovo;  
			}
			Yn[netlistLido[i].d][netlistLido[i].d]+=g;
			Yn[netlistLido[i].g][netlistLido[i].g]+=g;
			Yn[netlistLido[i].d][netlistLido[i].g]-=g;
			Yn[netlistLido[i].g][netlistLido[i].d]-=g;
			
			Yn[netlistLido[i].g][netlistLido[i].g]+=g;
			Yn[netlistLido[i].s][netlistLido[i].s]+=g;
			Yn[netlistLido[i].g][netlistLido[i].s]-=g;
			Yn[netlistLido[i].s][netlistLido[i].g]-=g;
			
			Yn[netlistLido[i].g][netlistLido[i].g]+=g;
			Yn[netlistLido[i].b][netlistLido[i].b]+=g;
			Yn[netlistLido[i].g][netlistLido[i].b]-=g;
			Yn[netlistLido[i].b][netlistLido[i].g]-=g;
			
		}
		if (mostraEstampas_PO)
		codigoDeErro = printaSNM_PO(Yn, &numVariaveis);
	}

	return OK;
}


//---------------------------------------------------------------------------


int
determinarModoOperacaoTrans(elemento netlistLido[MAX_ELEMENTOS], int * pPontoOperacao, double solucaoAnterior[MAX_NOS], int  i)
{
	int drainMaiorTensao;
	int aux;
	int pontoOperacao;
	double vDS;
	double vGS;
	double vBS;
	double vt;
	char tipoTransistor [MAX_TIPO_TRANS];
	
	vDS = 0.0;
	vGS = 0.0;
	vBS = 0.0;
	
	
	/* renata: fiz isso para consertar um bug de EOS, entendi pq o lanza nao usa sscanf é uma bosta mto instavel*/
	strncpy (tipoTransistor, netlistLido[i].tipoTrans,(size_t) MAX_TIPO_TRANS);
	tipoTransistor[MAX_TIPO_TRANS] = '\0';
	strncpy (netlistLido[i].tipoTrans,tipoTransistor,(size_t) MAX_TIPO_TRANS);
	netlistLido[i].tipoTrans[MAX_TIPO_TRANS] = '\0';	  

	if (solucaoAnterior[netlistLido[i].d] > solucaoAnterior[netlistLido[i].s])
	{
		drainMaiorTensao = 1;
	}	
	
	if ( strcmp(tipoTransistor,"NMOS") == 0)
	{
		if (!drainMaiorTensao)
		{
			aux =netlistLido[i].s;
			netlistLido[i].s =netlistLido[i].d;
			netlistLido[i].d=aux;
			netlistLido[i].inverteu +=1;
		}
	}
	else if (strcmp(tipoTransistor,"PMOS") == 0)
	{
		if (drainMaiorTensao)
		{
			aux =netlistLido[i].s;
			netlistLido[i].s =netlistLido[i].d;
			netlistLido[i].d=aux;
			netlistLido[i].inverteu +=1;
		}	  	
	}
	
	vDS = solucaoAnterior[netlistLido[i].d] - solucaoAnterior[netlistLido[i].s];
	vGS = solucaoAnterior[netlistLido[i].g] - solucaoAnterior[netlistLido[i].s];
	vt  = netlistLido[i].vtzeroTrans + netlistLido[i].lambdaTrans*(sqrt(netlistLido[i].phiTrans - vBS) - sqrt(netlistLido[i].phiTrans));
	
	if (strcmp(tipoTransistor, "NMOS") == 0)
	{ 
		if (vGS < vt) /* estava assim:  if (vGS <= vt) */
		{
			pontoOperacao = 0;
		}
		else if(vDS <= vGS - vt) /* estava assim: else if(vDS < vGS - vt) */
		{
			pontoOperacao = 1;	
		}
		else 
		{
			pontoOperacao = 2;
		}
	}
	else if (strcmp(tipoTransistor,"PMOS") == 0)
	{ 
		if (vGS > -vt) /* estava assim:  if (vGS >= vt) */
		{
			pontoOperacao = 0;
		}
		else if(-vDS <= (-vGS - vt)) /* estava assim:  else if(vDS > (vGS - vt)) */
		{ 
			pontoOperacao = 1;	
		}
		else 
		{ 
			pontoOperacao = 2;
		}
	}
	netlistLido[i].modoOperacao = pontoOperacao;
	*pPontoOperacao = pontoOperacao;
	return OK;	  
}
//---------------------------------------------------------------------------

int 
zeraSistema_PO(double sistema[MAX_NOS+1][MAX_NOS+2], int *pNumVariaveis)
{
	int i = 0;
	int j = 0;
	int numVariaveis;
	
	if (pNumVariaveis==NULL)
	return NUMERO_VARIAVEIS_INVALIDO;
	
	numVariaveis = *pNumVariaveis;
	
	for (i=0; i<=numVariaveis; i++)
	for (j=0; j<=numVariaveis+1; j++)
	sistema[i][j]=0.0;

	return OK;
}

//---------------------------------------------------------------------------

int
zeraSistema_JW(cppcomplex sistema[MAX_NOS+1][MAX_NOS+2], int *pNumVariaveis)
{
	int i;
	int numVariaveis;
	int j;
	cppcomplex complexado(0.0,0.0);
	
	
	if (pNumVariaveis==NULL)
	return NUMERO_VARIAVEIS_INVALIDO;
	
	numVariaveis = *pNumVariaveis;
	
	for (i=0; i<=numVariaveis; i++)
	for (j=0; j<=numVariaveis+1; j++)
	{
		sistema[i][j] = complexado;
	}
	
	return OK;
}

//---------------------------------------------------------------------------

int 
printaSNM_PO(double sistema[MAX_NOS+1][MAX_NOS+2], int *pNumVariaveis)
{
	int i;
	int j;
	int numVariaveis;
	char buf[1000];
	char tmp [1000];
	
	numVariaveis = *pNumVariaveis;


	for (i=1; i<=numVariaveis; i++)
	{
		strcpy(buf,"");
		for (j=1; j<=numVariaveis+1; j++) {
			if (sistema[i][j]!=0) sprintf(tmp,"%+6.2g ",sistema[i][j]);
			else sprintf(tmp,"......... ");
			strcat(buf,tmp);
		}
		printf(buf);
	}



	return OK;
}

//---------------------------------------------------------------------------

int 
resolverSNM_PO(double sistema[MAX_NOS+1][MAX_NOS+2], int *pNumVariaveis)
{
	int i;
	int j;
	int l; 
	int a;
	double t;
	double p;
	int numVariaveis;
	
	numVariaveis = *pNumVariaveis;

	for (i=1; i<=numVariaveis; i++) 
	{	
		t=0.0;
		a=i;
		for (l=i; l<=numVariaveis; l++) 
		{
			if (fabs(sistema[l][i])>fabs(t)) 
			{
				a=l;
				t=sistema[l][i];
			}
		}
		if (i!=a) 
		{
			for (l=1; l<=numVariaveis+1; l++)
			{
				p=sistema[i][l];
				sistema[i][l]=sistema[a][l];
				sistema[a][l]=p;
			}
		}
		if (fabs(t)<TOLG)
		{
			printf("Sistema singular\n");
			return SISTEMA_SINGULAR;
		}
		for (j=numVariaveis+1; j>0; j--) 
		{  /* Basta j>i em vez de j>0 */
			sistema[i][j]/= t;
			p=sistema[i][j];
			if (p!=0.0)  /* Evita operacoes com zero */
			for (l=1; l<=numVariaveis; l++) 
			{
				if (l!=i)
				sistema[l][j]-=sistema[l][i]*p;
			}
		}
	}
	return OK;
}


//---------------------------------------------------------------------------

int
inicializacaoAleatoria(double solucaoAnterior[MAX_NOS], int *pNumNos , int *pNumVariaveis, double erros[MAX_NOS+1])
{
	double valor;
	int i;
	int numVariaveis;
	int numNos;
	
	numNos = *pNumNos;
	numVariaveis = *pNumVariaveis;

	srand ((unsigned) time(NULL));
	
	for (i=1; i<=numVariaveis;i++)
	{
		if(i <= numNos)
		{
			if (erros[i] > MAX_ERRO)
			{
				
				solucaoAnterior[i] = (((rand() % 2000)/100.0)-10);
			}	
		}
		else /* isso eh para iniciar as correntes de -1 a 1 */
		{
			if(erros[i] > MAX_ERRO)
			{
				solucaoAnterior[i]=(((rand() % 200)/100.0)-1);
			}
		}	
	}
	return OK;
}


//---------------------------------------------------------------------------

int
salvaUltimaIteracao(double Yn[MAX_NOS+1][MAX_NOS+2], double solucaoAnterior[MAX_NOS], int *pNumVariaveis)
{
	int i;
	int numVariaveis;
	numVariaveis = *pNumVariaveis;
	
	for (i=1; i<= numVariaveis; i++)
	{ 
		solucaoAnterior[i] = Yn[i][numVariaveis+1];
	}
	return OK;
}

//---------------------------------------------------------------------------

int 
testeDeConvergencia (double Yn[MAX_NOS+1][MAX_NOS+2], double  solucaoAnterior[MAX_NOS], int *pNumVariaveis, double erros[MAX_NOS+1],  int iteracao)
{
	int i, convergiu; /*convergiu = 1, nao convergiu = 0 */
	int numVariaveis;
	numVariaveis = *pNumVariaveis;
	convergiu = 1;
	
	for (i=1; (i<=numVariaveis);i++) 
	{    
		if (fabs(Yn[i][numVariaveis+1])>X) 
		{  /* Se o valor e grande usa erro relativo*/
			erros[i] = X*fabs((solucaoAnterior[i]-Yn[i][numVariaveis+1])/Yn[i][numVariaveis+1]);
			if (erros[i] > MAX_ERRO_RELATIVO )
			convergiu = 0;
		}
		else 
		{  /* Se o valor e pequeno usa erro absoluto */
			erros[i]=fabs(solucaoAnterior[i]-Yn[i][numVariaveis+1]);
			if (erros[i] > MAX_ERRO)
			convergiu = 0;
		}

	}

	
	for (i = 1; i<=numVariaveis; i++)
	if (convergiu == 0)
	return NAO_CONVERGIU;

	return CONVERGIU;
	
}


//---------------------------------------------------------------------------

int
newtonRaphson_PO (double Yn[MAX_NOS+1][MAX_NOS+2], double solucaoAnterior[MAX_NOS], elemento netlistLido[MAX_ELEMENTOS], 
int *pNumElementos, int *pNumVariaveis, int *pNumNos, int numTransistores, int vetorIndicesTrans[MAX_ELEMENTOS])
{
	int iteracao;
	int i, j;
	int init;
	int codigoDeErro;
	int convergiu; /* 0 falso, 1 verdadeiro*/
	int numVariaveis;
	int numNos;
	int numElementos;
	int indice, contador;
	double erros[MAX_NOS + 1];
	double vDS, vGS, vBS;
	
	/*Inicializacao das variaveis de controle*/
	numNos = *pNumNos;
	numVariaveis = *pNumVariaveis;
	numElementos = *pNumElementos; 
	
	/* Inicializacao das variaveis do sistema */
	codigoDeErro = zeraSistema_PO(Yn, &numVariaveis);
	codigoDeErro = salvaUltimaIteracao(Yn, solucaoAnterior, &numVariaveis);
	
	
	/* incializa com 0.1 para a primeira iteracao */
	for (i=1; i<=numVariaveis; i++)
	solucaoAnterior[i] = aproximacaoInicialDasCorrentes;

	

	for (init=0; init <= MAX_INIT; init++)
	{ 
		//printf ("\n ~~ Inicializacao # %d \n", init);
		if (init != 0)
		{ /*na primeira iteracao do for do init nao entra aqui, usa o valor 0.1, depois nas outras entra */
			codigoDeErro = inicializacaoAleatoria(solucaoAnterior, &numNos, &numVariaveis, erros);
		}
		
		
		for (iteracao = 1; iteracao <= MAX_ITERACOES  > TOLERANCIA; iteracao++)
		{ 
			//printf (" \n Iteracao # %d \n", iteracao);
			codigoDeErro = zeraSistema_PO(Yn, &numVariaveis);
			codigoDeErro = montaSNM_PO (Yn, netlistLido, &numElementos, &numVariaveis, &numNos, solucaoAnterior);
			codigoDeErro = resolverSNM_PO(Yn, &numVariaveis);
			convergiu = testeDeConvergencia(Yn, solucaoAnterior, &numVariaveis, erros, iteracao);	
			//printf("\n Solucao Atual, Anterior e Erro. \n");
			if ((mostraResultados) && (!mostraResultadosAndErros))
			{
				printf (" \n Iteracao # %d \n", iteracao);
				for (j=1;j<=numVariaveis;j++)
				printf(" Variavel #%d: %lg \n",j, Yn[j][numVariaveis +1]);	
			}
			if (mostraResultadosAndErros || (mostraResultadosAndErros && mostraResultados) )
			{
				printf (" \n Iteracao # %d \n", iteracao);
				for (j=1;j<=numVariaveis;j++)
				printf(" Variavel #%d: %lg Valor anterior: %lg Erro: %lg \n",j, Yn[j][numVariaveis +1], solucaoAnterior[j], erros[j]);
			}
			/* 

			for (contador=1; contador<=numTransistores; contador++)
			{
				indice = vetorIndicesTrans[contador];
				printf("\n %s: m.o.=%i, ds=%i, gmGS=%lg , gDS=%lg, gmBS=%lg , IO =%+lg, vDS=%+lg, vGS=%+lg, vBS=%+lg\n ",netlistLido[indice].nome,netlistLido[indice].modoOperacao,
				netlistLido[indice].inverteu,netlistLido[indice].gmGS,netlistLido[indice].gmDS,netlistLido[indice].gmBS,netlistLido[indice].correnteDCmos,
				netlistLido[indice].vDS,netlistLido[indice].vGS,netlistLido[indice].vBS);
			} */
			
			/*getch();*/
			
			codigoDeErro = salvaUltimaIteracao(Yn, solucaoAnterior, &numVariaveis); 
			
			if (convergiu == CONVERGIU)
			{
				printf ("Sistema convergiu com %i iteracoes e reinicios %i\n", iteracao, init);
				printf("\n");
				for (j=1;j<=numVariaveis;j++)
				{
					printf(" Variavel #%d: %lg \n",j, Yn[j][numVariaveis +1]);	
				}
				
				return OK;
			}
			/*codigoDeErro = printaSNM_PO(Yn, &numVariaveis);*/
		}
	}

	printf("Sistema nao convergiu com iteracoes: %i, reinicios: %i\n",iteracao-1, init-1);
	return NAO_CONVERGIU;
}
//---------------------------------------------------------------------------

int
montaSNM_JW (double frequenciaAngular, cppcomplex Yn_JW[MAX_NOS+1][MAX_NOS+2], elemento netlistLido[MAX_ELEMENTOS],
int * pNumElementos, int * pNumVariaveis, int * pNumNos, double solucaoAnterior[MAX_NOS])
{
	int numVariaveis;
	int numElementos;			
	char tipoDoElemento;
	cppcomplex g_jw(0.0,0.0);
	cppcomplex Cgs_jw(0.0,0.0);
	cppcomplex Cgd_jw(0.0,0.0);
	cppcomplex Cbg_jw(0.0,0.0); 
	int codigoDeErro = OK;

	int i;
	int indicePar;
	
	numVariaveis = *pNumVariaveis;
	numElementos = *pNumElementos;	
	
	double g;
	
	for (i=1; i<=numElementos; i++)
	{
		tipoDoElemento = netlistLido[i].nome[0];
		if (tipoDoElemento=='R')
		{
			g_jw = 1/netlistLido[i].valor;
			Yn_JW[netlistLido[i].a][netlistLido[i].a] += g_jw;
			Yn_JW[netlistLido[i].b][netlistLido[i].b] += g_jw;
			Yn_JW[netlistLido[i].a][netlistLido[i].b] -= g_jw;
			Yn_JW[netlistLido[i].b][netlistLido[i].a] -= g_jw;
		}
		else if (tipoDoElemento=='L')
		{
			g_jw = (I*frequenciaAngular*netlistLido[i].valor); 
			Yn_JW[netlistLido[i].a][netlistLido[i].x] += 1;
			Yn_JW[netlistLido[i].b][netlistLido[i].x] -= 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].a] -= 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].b] += 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].x] += g_jw;
			if (netlistLido[i].m > 0)
			{
				indicePar = netlistLido[i].par;
				Yn_JW[netlistLido[i].x][netlistLido[indicePar].x] += I*frequenciaAngular*netlistLido[i].m;	   
			}
			
		}
		else if (tipoDoElemento=='C')
		{
			g_jw = I*frequenciaAngular*netlistLido[i].valor; 
			Yn_JW[netlistLido[i].a][netlistLido[i].a] += g_jw;
			Yn_JW[netlistLido[i].b][netlistLido[i].b] += g_jw;
			Yn_JW[netlistLido[i].a][netlistLido[i].b] -= g_jw;
			Yn_JW[netlistLido[i].b][netlistLido[i].a] -= g_jw;
		}
		else if (tipoDoElemento=='G')
		{
			g_jw = netlistLido[i].valor;
			Yn_JW[netlistLido[i].a][netlistLido[i].c] += g_jw;
			Yn_JW[netlistLido[i].b][netlistLido[i].d] += g_jw;
			Yn_JW[netlistLido[i].a][netlistLido[i].d] -= g_jw;
			Yn_JW[netlistLido[i].b][netlistLido[i].c] -= g_jw;
		}
		else if (tipoDoElemento=='I')
		{ 
			g_jw = netlistLido[i].valorAC;
			Yn_JW[netlistLido[i].a][numVariaveis+1] -= g_jw;
			Yn_JW[netlistLido[i].b][numVariaveis+1] += g_jw;
		}
		else if (tipoDoElemento=='V')
		{
			g_jw = netlistLido[i].valorAC;
			Yn_JW[netlistLido[i].a][netlistLido[i].x] += 1;
			Yn_JW[netlistLido[i].b][netlistLido[i].x] -= 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].a] -= 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].b] += 1;
			Yn_JW[netlistLido[i].x][numVariaveis+1] -= g_jw;
		}
		else if (tipoDoElemento=='E')
		{
			g_jw = netlistLido[i].valor;
			Yn_JW[netlistLido[i].a][netlistLido[i].x] += 1;
			Yn_JW[netlistLido[i].b][netlistLido[i].x] -= 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].a] -= 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].b] += 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].c] += g_jw;
			Yn_JW[netlistLido[i].x][netlistLido[i].d] -= g_jw;
		}
		else if (tipoDoElemento=='F')
		{
			g_jw = netlistLido[i].valor;
			Yn_JW[netlistLido[i].a][netlistLido[i].x] += g_jw;
			Yn_JW[netlistLido[i].b][netlistLido[i].x] -= g_jw;
			Yn_JW[netlistLido[i].c][netlistLido[i].x] += 1;
			Yn_JW[netlistLido[i].d][netlistLido[i].x] -= 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].c] -= 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].d] += 1;
		}
		else if (tipoDoElemento=='H')
		{
			g_jw = netlistLido[i].valor;
			Yn_JW[netlistLido[i].a][netlistLido[i].y] += 1;
			Yn_JW[netlistLido[i].b][netlistLido[i].y] -= 1;
			Yn_JW[netlistLido[i].c][netlistLido[i].x] += 1;
			Yn_JW[netlistLido[i].d][netlistLido[i].x] -= 1;
			Yn_JW[netlistLido[i].y][netlistLido[i].a] -= 1;
			Yn_JW[netlistLido[i].y][netlistLido[i].b] += 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].c] -= 1;
			Yn_JW[netlistLido[i].x][netlistLido[i].d] += 1;
			Yn_JW[netlistLido[i].y][netlistLido[i].x] += g_jw;
		}
		else if (tipoDoElemento=='O') 
		{
			Yn_JW[netlistLido[i].a][netlistLido[i].x]+=1;
			Yn_JW[netlistLido[i].b][netlistLido[i].x]-=1;
			Yn_JW[netlistLido[i].x][netlistLido[i].c]+=1;
			Yn_JW[netlistLido[i].x][netlistLido[i].d]-=1;
		}
		else if (tipoDoElemento == 'M')
		{
			if (usarResistenciaAberto)
			{
				g = 1/RESISTENCIA_ABERTO;
			}
			else
			{
				g = 0;
			}
			
			Yn_JW[netlistLido[i].d][netlistLido[i].d]+= g +netlistLido[i].gmDS;
			Yn_JW[netlistLido[i].d][netlistLido[i].s]+= -netlistLido[i].gmDS -netlistLido[i].gmGS -netlistLido[i].gmBS;
			Yn_JW[netlistLido[i].d][netlistLido[i].g]+= -g +netlistLido[i].gmGS;
			Yn_JW[netlistLido[i].d][netlistLido[i].b]+= netlistLido[i].gmBS;
			Yn_JW[netlistLido[i].s][netlistLido[i].d]+= -netlistLido[i].gmDS;
			Yn_JW[netlistLido[i].s][netlistLido[i].s]+= g+netlistLido[i].gmDS +netlistLido[i].gmGS +netlistLido[i].gmBS;
			Yn_JW[netlistLido[i].s][netlistLido[i].g]+= -g -netlistLido[i].gmGS;
			Yn_JW[netlistLido[i].s][netlistLido[i].b]+= -netlistLido[i].gmBS;
			Yn_JW[netlistLido[i].g][netlistLido[i].d]+= -g;
			Yn_JW[netlistLido[i].g][netlistLido[i].s]+= -g;
			Yn_JW[netlistLido[i].g][netlistLido[i].g]+= 3*g;
			Yn_JW[netlistLido[i].g][netlistLido[i].b]+= -g;
			Yn_JW[netlistLido[i].b][netlistLido[i].g]+= -g;
			Yn_JW[netlistLido[i].b][netlistLido[i].b]+= g;
			
			/* coloquei as capacitancias */
			
			Cgs_jw = I*frequenciaAngular*netlistLido[i].Cgs;
			//	printf("na funcao monta estampa imag%f real%f", Cgs_jw.imag(),Cgs_jw.real() ) ;
			Yn_JW[netlistLido[i].g][netlistLido[i].g] += Cgs_jw;
			Yn_JW[netlistLido[i].s][netlistLido[i].s] += Cgs_jw;
			Yn_JW[netlistLido[i].g][netlistLido[i].s] -= Cgs_jw;
			Yn_JW[netlistLido[i].s][netlistLido[i].g] -= Cgs_jw;
			
			
			
			Cgd_jw = I*frequenciaAngular*netlistLido[i].Cgd; 
			//	printf("na funcao monta estampa Cgd imag%f real%f", Cgd_jw.imag(),Cgd_jw.real() ) ;
			Yn_JW[netlistLido[i].g][netlistLido[i].g] += Cgd_jw;
			Yn_JW[netlistLido[i].d][netlistLido[i].d] += Cgd_jw;
			Yn_JW[netlistLido[i].g][netlistLido[i].d] -= Cgd_jw;
			Yn_JW[netlistLido[i].d][netlistLido[i].g] -= Cgd_jw;
			
			Cbg_jw = I*frequenciaAngular*netlistLido[i].Cbg;
			//	printf("na funcao monta estampa Cbg imag%f real%f", Cbg_jw.imag(),Cbg_jw.real() ) ; 
			Yn_JW[netlistLido[i].b][netlistLido[i].b] += Cbg_jw;
			Yn_JW[netlistLido[i].g][netlistLido[i].g] += Cbg_jw;
			Yn_JW[netlistLido[i].b][netlistLido[i].g] -= Cbg_jw;
			Yn_JW[netlistLido[i].g][netlistLido[i].b] -= Cbg_jw;
			
		}
		if(mostraEstampa_JW)
		codigoDeErro = printaSNM_JW(Yn_JW, &numVariaveis);

	}
	return OK;	
}


//---------------------------------------------------------------------------


int resolverSNM_JW(cppcomplex sistema[MAX_NOS+1][MAX_NOS+2], int *pNumVariaveis)
{
	int i;
	int j;
	int l;
	int a;
	int numVariaveis;
	

	cppcomplex t(0.0,0.0);
	cppcomplex p(0.0,0.0);
	
	numVariaveis = *pNumVariaveis;


	for (i=1; i<=numVariaveis; i++) 
	{
		t=0.0;
		a=i;
		for (l=i; l<=numVariaveis; l++) {
			if (moduloComplexo(sistema[l][i])>moduloComplexo(t)) {
				a=l;
				t=sistema[l][i];
			}
		}
		if (i!=a) {
			for (l=1; l<=numVariaveis+1; l++) {
				p=sistema[i][l];
				sistema[i][l]=sistema[a][l];
				sistema[a][l]=p;
			}
		}
		double modulo = 0;
		//	printf("NA funcao resolver: parte real %f parte imag %f modulo %f\n", t.real(), t.imag(), moduloComplexo(t));
		
		if ((modulo = moduloComplexo(t))<TOLG) {
			printf("Sistema singular\n");
			return SISTEMA_SINGULAR;
		}
		for (j=numVariaveis+1; j>0; j--) {  /* Basta j>i em vez de j>0 */
			sistema[i][j]/= t;
			p=sistema[i][j];
			if (p!=(0.0,0.0))  /* Evita operacoes com zero */
			for (l=1; l<=numVariaveis; l++)
			{
				if (l!=i)
				sistema[l][j]-=sistema[l][i]*p;
			}
		}

	}
	return OK;
}


//---------------------------------------------------------------------------
int
printaSNM_JW(cppcomplex sistema[MAX_NOS+1][MAX_NOS+2], int *pNumVariaveis)
{
	int numVariaveis;
	
	numVariaveis = *pNumVariaveis;
	int k;
	int j;
	char buf[1000];
	char tmp [1000];
	
	
	for (k=1; k<=numVariaveis; k++) 
	{
		strcpy(buf,"");
		for (j=1; j<=numVariaveis+1; j++) {
			if (abs(sistema[k][j])!=0) sprintf(tmp,"%+6.2e%+6.2ei ",real(sistema[k][j]),imag(sistema[k][j]));
			else sprintf(tmp,"......... ......... ");
			strcat(buf,tmp);
		}
		printf(buf);
	}
	
	return OK;
}
//---------------------------------------------------------------------------

int
printaEstampa(int indice, elemento netlist[MAX_ELEMENTOS], int numVariaveis, double Yn[MAX_NOS+1][MAX_NOS+2])
{
	
	int k,j;
	
	printf("Sistema apos a estampa de %s\n",netlist[indice].nome);
	for (k=1; k<=numVariaveis; k++) {
		for (j=1; j<=numVariaveis+1; j++)
		if (Yn[k][j]!=0) printf("%+.2g ",Yn[k][j]);
		else printf(" .... ");
		printf("\n");
	}
	getch();

	return OK;
}

//---------------------------------------------------------------------------

int
calculaCapacitancias(elemento netlist[MAX_ELEMENTOS], int indicesTrans[MAX_ELEMENTOS], int numTransistores)
{
	
	double mi, Cox, cgs,cgd;
	int i, indice;
	char tipoTransistor [MAX_TIPO_TRANS];
	
	for (i=1; i <= numTransistores; i++)
	{
		indice = indicesTrans[i];
		
		
		if ( netlist[indice].tipoTrans[0] == 'N')
		{
			/* printf("tem mi 0.05 \n"); */
			mi = 0.05;
		}	
		else
		{
			/* printf("tem mi 0.02 \n"); */
			mi = 0.02;
		}
		
		
		Cox = ((2*netlist[indice].kTrans) / mi);
		
		
		if (netlist[indice].modoOperacao == 0) /* corte */
		{
			//printf("capacitancia: entrou no corte \n");
			netlist[indice].Cgs = Cox * netlist[indice].larguraTrans * netlist[indice].ldTrans;
			netlist[indice].Cgd = Cox * netlist[indice].larguraTrans * netlist[indice].ldTrans;
			netlist[indice].Cbg = Cox * netlist[indice].larguraTrans * netlist[indice].comprimentoTrans;
		}
		if (netlist[indice].modoOperacao == 1) /* regiao omica*/
		{
			//printf("capacitancia: entrou em triodo \n");
			netlist[indice].Cgs = Cox * netlist[indice].larguraTrans * netlist[indice].ldTrans + 0.5* Cox * netlist[indice].larguraTrans * netlist[indice].comprimentoTrans;
			netlist[indice].Cgd = Cox * netlist[indice].larguraTrans * netlist[indice].ldTrans + 0.5* Cox * netlist[indice].larguraTrans * netlist[indice].comprimentoTrans;
			netlist[indice].Cbg = 0.0;
		}
		if (netlist[indice].modoOperacao == 2) /* saturacao */
		{
			//printf("capacitancia: entrou em saturacao \n");
			netlist[indice].Cgs = Cox * netlist[indice].larguraTrans * netlist[indice].ldTrans + ((2*Cox)/3)*netlist[indice].larguraTrans * netlist[indice].comprimentoTrans;
			netlist[indice].Cgd = Cox * netlist[indice].larguraTrans * netlist[indice].ldTrans;  
			netlist[indice].Cbg = 0.0;	
		}
	}
	return OK;
}
//---------------------------------------------------------------------------

int
escreverTabelaParametrosTransistor(elemento netlist[MAX_ELEMENTOS], int indicesTrans[MAX_ELEMENTOS], int numTransistores,  char * nomeArquivo)
{

	int i,indice;
	FILE *arquivo;
	char *nomeArquivoTabPT;
	char inverteu[20];
	char modoOperacao[20];



	/*inicializar variaveis*/
	nomeArquivoTabPT = strtok(nomeArquivo,".");
	strcat (nomeArquivoTabPT,"_PT.tab");

	if (!(arquivo = fopen (nomeArquivoTabPT, "w")))
	{
		printf("Erro ao abrir o arquivo #%i: %s\n", errno, strerror(errno));
		return ERRO_ABRINDO_ARQUIVO;
	}

	fprintf (arquivo, "Parametros transistores \n");
	for (i = 1; i <= numTransistores; i++)
	{
		indice = indicesTrans[i];
		if ((netlist[indice].inverteu%2)>0)
		{
			strcpy(inverteu,"invertido");
		}
		else
		{
			strcpy(inverteu,"normal");
		}
		switch(netlist[indice].modoOperacao) 
		{
		case 0:
			strcpy(modoOperacao,"corte");
			break;
		case 1:
			strcpy(modoOperacao,"triodo");
			break;
		case 2:
			strcpy(modoOperacao,"saturado");
			break;
		}
		if (mostraMOS)
		{
			printf("Escrevendo arquivo %s \n", nomeArquivoTabPT);
			fprintf(arquivo,  "%s: %c %s %s Gm=%lg Gds=%lg Gmb=%lg Cgs=%lg Cgd=%lg Cgb=%lg Vgs=%lg Vds=%lg Vbs=%lg \n",netlist[indice].nome, netlist[indice].tipoTrans[0], inverteu, modoOperacao, netlist[indice].gmGS, netlist[indice].gmDS, netlist[indice].gmBS, netlist[indice].Cgs, netlist[indice].Cgd, netlist[indice].Cbg, netlist[indice].vGS, netlist[indice].vDS,	netlist[i].vBS);
			
			printf("%s: %c %s %s Gm=%lg Gds=%lg Gmb=%lg Cgs=%lg Cgd=%lg Cgb=%lg Vgs=%lg Vds=%lg Vbs=%lg \n",netlist[indice].nome, netlist[indice].tipoTrans[0], inverteu,	modoOperacao, netlist[indice].gmGS, netlist[indice].gmDS, netlist[indice].gmBS, netlist[indice].Cgs, netlist[indice].Cgd, netlist[indice].Cbg, netlist[indice].vGS, netlist[indice].vDS, netlist[indice].vBS);
		}
		else if(salvaParamTrans)
		{
			
			fprintf(arquivo,  "%s: %c %s %s Gm=%lg Gds=%lg Gmb=%lg Cgs=%lg Cgd=%lg Cgb=%lg Vgs=%lg Vds=%lg Vbs=%lg \n",netlist[indice].nome, netlist[indice].tipoTrans[0], inverteu, modoOperacao, netlist[indice].gmGS, netlist[indice].gmDS, netlist[indice].gmBS, netlist[indice].Cgs, netlist[indice].Cgd, netlist[indice].Cbg, netlist[indice].vGS, netlist[indice].vDS,	netlist[i].vBS);
			
		}
	}
	fclose(arquivo);
	return OK;
}

//---------------------------------------------------------------------------

double
moduloComplexo (cppcomplex a)
{ 
	double b;
	
	b = (a.real()*a.real() + a.imag()*a.imag());
	b = sqrt(b);
	return b;
}


//---------------------------------------------------------------------------

int
quantidadeTrans(elemento netlist[MAX_ELEMENTOS] , int numElementos, int vetorIndicesTrans[MAX_ELEMENTOS])
{
	int contador;
	int numeroDeTransistores = 0;
	char tipoDoElemento;


	for (contador=1; contador<=numElementos;) 
	{
		tipoDoElemento = netlist[contador].nome[0];
		if (tipoDoElemento=='M') 
		{  
			numeroDeTransistores += 1;
			vetorIndicesTrans[numeroDeTransistores] = contador;
		}
		contador++;
	}
	
	//printf("No fim da funcao a quantidade de transistor eh: %i", numeroDeTransistores);
	return numeroDeTransistores;
}


//---------------------------------------------------------------------------
//COMECO DA ~~~~~ MAIN ~~~~~
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
: TForm(Owner)
{
	analiseAC.comeco = 1;
	analiseAC.escala = LIN;
	analiseAC.fim = 10000;
	analiseAC.numPontos = 1000;
	aproximacaoInicialDasCorrentes = 0.1;
	MAX_ERRO = MAX_ERRO_DEFAULT;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Abrir1Click(TObject *Sender)
{
	// variaveis da main

	char nomeArquivo[COMPRIMENTO_MAX_NOME_ARQUIVO];
	int codigoDeErro = 0 ; 
	int numElementos; /* Elementos */
	int numVariaveis; /* Variaveis */
	int numNos; /* Nos */
	int numTransistores = 0;
	int modoOperacao; /* 0: corte 1: r. ômica 2: saturação*/
	int numPontos, ponto;
	int contador;
	int indice;
	int verificaElementoReativo;
	
	elemento netlistLido[MAX_ELEMENTOS];
	double Yn[MAX_NOS+1][MAX_NOS+2];	
	double solucaoAnterior[MAX_NOS];
	double frequenciaAngular, passoAngular;
	cppcomplex Yn_JW [MAX_NOS+1][MAX_NOS+2];
	int vetorIndicesTrans[MAX_ELEMENTOS];
	double vDS, vGS, vBS;
	char listaDeNos[MAX_NOS + 1][MAX_NOME + 2];
	
	/* Inicializacoes */
	verificaElementoReativo = SEM_REATIVO;
	numElementos = 0;
	numVariaveis = 0;
	numNos       = 0;
	modoOperacao = SEM_TRANSISTOR;
	
	contador = 0;
	frequenciaAngular = 1;
	

	
	strcpy(nomeArquivo,"1");
	
	/* inicio do programa */
	
	

	

leitura:
	if (!OpenDialog1->Execute()) return;
	strcpy(nomeArquivo,OpenDialog1->FileName.c_str());
	codigoDeErro = lerNetlist (listaDeNos, nomeArquivo, &numElementos, &numVariaveis, &numNos, netlistLido, &analiseAC, &modoOperacao);
	if (codigoDeErro == 2)
	{
		goto leitura;
	}
	

	codigoDeErro = zeraSistema_PO(Yn, &numVariaveis);
	numTransistores = quantidadeTrans(netlistLido, numElementos, vetorIndicesTrans);

	/*-----------------------------------ANALISE DE PONTO DE OPERACAO------------------------------------------------------------*/
	if (modoOperacao == SEM_TRANSISTOR)
	{ /*entra nesse modo se não tiver transistor no circuito, essa variavel eh setada no ler netlis do transistor*/
		codigoDeErro = montaSNM_PO (Yn, netlistLido, &numElementos, &numVariaveis, &numNos, solucaoAnterior);
		if (mostraEstampas_PO)
		codigoDeErro = printaSNM_PO(Yn, &numVariaveis);
		
		printf("\n\n Calculando ponto de operacao: \n");
		codigoDeErro = resolverSNM_PO(Yn, &numVariaveis);
		codigoDeErro = printaSNM_PO(Yn, &numVariaveis); 		
	}
	else 
	{ /* se tem transistor */
		printf("\n\n Calculando ponto de operacao: \n");

		codigoDeErro = newtonRaphson_PO(Yn, solucaoAnterior, netlistLido, &numElementos, &numVariaveis, &numNos, numTransistores,vetorIndicesTrans);
		if (codigoDeErro != 0)
		{
			printf("Na main: sistema nao convergiu\n");
			exit(NAO_CONVERGIU);
		}
		codigoDeErro = calculaCapacitancias(netlistLido, vetorIndicesTrans, numTransistores);
		
		/* Se "inverteu" for impar, os terminais drain e source estao trocados. */
		for (contador=1; contador<=numTransistores; contador++)
		{
			indice = vetorIndicesTrans[contador];
			vDS = solucaoAnterior[netlistLido[indice].d] - solucaoAnterior[netlistLido[indice].s];
			vGS = solucaoAnterior[netlistLido[indice].g] - solucaoAnterior[netlistLido[indice].s];
			vBS = solucaoAnterior[netlistLido[indice].b] - solucaoAnterior[netlistLido[indice].s];
			//printf("\n %s: m.o.=%i, ds=%i, gmGS=%.2g , gDS=%.2g, gmBS=%.2g , IO =%+.2g, vDS=%+.2g, vGS=%+.2g, vBS=%+.2g \n ",netlistLido[indice].nome,netlistLido[indice].modoOperacao,
			//netlistLido[indice].inverteu,netlistLido[indice].gmGS,netlistLido[indice].gmDS,netlistLido[indice].gmBS,netlistLido[indice].correnteDCmos,
			//vDS,vGS,vBS);
			//printf("\n Cgs=%.2g, Cgd=%.2g, Cbg=%.2g \n", netlistLido[indice].Cgs, netlistLido[indice].Cgd, netlistLido[indice].Cbg );
		}
	}	
	
	/* --------------------------------ANALISE JW-------------------------------------------------------------*/
	/*evitar divisoes por zero*/
	if (analiseAC.comeco == 0)
	frequenciaAngular = 1e-4*2*PI;
	else 	
	frequenciaAngular = analiseAC.comeco *2*PI;
	
	numPontos = analiseAC.numPontos;
	ponto = 0;
	
	if (analiseAC.escala == 1)
	{  	
		passoAngular = pow (2, 1/(double) numPontos);
		numPontos = analiseAC.numPontos*floor((log10 (analiseAC.fim/analiseAC.comeco)/log10(2) ));
	}	
	if (analiseAC.escala == 2)
	{ 
		passoAngular = pow (10, 1/(double) numPontos);
		numPontos = floor(log10(analiseAC.fim/analiseAC.comeco))*analiseAC.numPontos;	
	}
	
	while (frequenciaAngular < (analiseAC.fim*2*PI))
	{ 
		codigoDeErro = zeraSistema_JW (Yn_JW, &numVariaveis);
		codigoDeErro = montaSNM_JW (frequenciaAngular, Yn_JW, netlistLido, &numElementos, &numVariaveis, &numNos, solucaoAnterior);
		codigoDeErro = resolverSNM_JW (Yn_JW, &numVariaveis);
		codigoDeErro = escreverTabela (listaDeNos, ponto, numPontos, Yn_JW, &numVariaveis, &frequenciaAngular, nomeArquivo, netlistLido);
		
		/*Passo e frequencia para cada tipo de escala*/ 
		if (analiseAC.escala == 0)
		{
			passoAngular = ((analiseAC.fim - analiseAC.comeco)/(analiseAC.numPontos-1))*2*PI;
			frequenciaAngular+= passoAngular;
		}
		if (analiseAC.escala == 1)
		{  	
			passoAngular = pow (2, 1/(double) (analiseAC.numPontos-1));
			frequenciaAngular = analiseAC.comeco * pow (passoAngular,(ponto+1))*2*PI;
		}	
		if (analiseAC.escala == 2)
		{ 
			passoAngular = pow (10, 1/(double) (analiseAC.numPontos-1));
			frequenciaAngular = analiseAC.comeco * pow (passoAngular,ponto+1)*2*PI;
		}
		ponto++;
	}
	if (salvaPO)
	codigoDeErro = escreverTabelaPO (listaDeNos, Yn, &numVariaveis, nomeArquivo, netlistLido);

	//printf("numero de transistor: %i ", numTransistores);
	if (salvaParamTrans || mostraMOS)
	codigoDeErro = escreverTabelaParametrosTransistor(netlistLido, vetorIndicesTrans, numTransistores, nomeArquivo);
	
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
	printf("Versão %s",versao);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm1::MostrarparametrosMOS1Click(TObject *Sender)
{
	MostrarparametrosMOS1->Checked=!MostrarparametrosMOS1->Checked;
	mostraMOS=MostrarparametrosMOS1->Checked;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm1::MostrarEstampasPOClick(TObject *Sender)
{
	MostraEstampasPO->Checked=!MostraEstampasPO->Checked;
	mostraEstampas_PO=MostraEstampasPO->Checked;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TForm1::MostrarEstampasAC1Click(TObject *Sender)
{
	MostrarEstampasAC1->Checked=!MostrarEstampasAC1->Checked;
	mostraEstampa_JW=MostrarEstampasAC1->Checked;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm1::MostrarConvergencia1Click(TObject *Sender)
{
	MostrarConvergencia1->Checked=!MostrarConvergencia1->Checked;
	mostraResultados=MostrarConvergencia1->Checked;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm1::MostrarErros1Click(TObject *Sender)
{
	MostrarErros1->Checked=!MostrarErros1->Checked;
	mostraResultadosAndErros=MostrarErros1->Checked;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm1::SalvaParamTrans1Click(TObject *Sender)
{
	SalvaParamTrans1->Checked=!SalvaParamTrans1->Checked;
	salvaParamTrans=SalvaParamTrans1->Checked;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm1::SalvaPO1Click(TObject *Sender)
{
	SalvaPO1->Checked=!SalvaPO1->Checked;
	salvaPO=SalvaPO1->Checked;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm1::ResistenciaAberto01Click(TObject *Sender)
{
	ResistenciaAberto01->Checked=!ResistenciaAberto01->Checked;
	usarResistenciaAberto=ResistenciaAberto01->Checked;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TForm1::Convergencia1Click(TObject *Sender)
{
	Form2->Show();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TForm1::Plot1Click(TObject *Sender)
{
	Form3->Show();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TForm1::Sobre1Click(TObject *Sender)
{
	Form4->Show();
}
//---------------------------------------------------------------------------



//FORM 2
//---------------------------------------------------------------------------
void __fastcall TForm2::erroMaxMenuChange(TObject *Sender)
{
	MAX_ERRO=erroMaxMenu->Text.ToDouble();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm2::aproxInicialCorrentesChange(TObject *Sender)
{
	aproximacaoInicialDasCorrentes=aproxInicialCorrentes->Text.ToDouble();

}
//---------------------------------------------------------------------------






//FORM 3
//---------------------------------------------------------------------------
void __fastcall TForm3::numPontosMenu1(TObject *Sender)
{
	analiseAC.numPontos=numPontosMenu->Text.ToDouble();

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm3::freInicialMenu1(TObject *Sender)
{
	analiseAC.comeco=freInicialMenu->Text.ToDouble();

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm3::freFinalMenu1(TObject *Sender)
{
	analiseAC.fim=freFinalMenu->Text.ToDouble();

}
//---------------------------------------------------------------------------
void __fastcall TForm3::ComboBox11(TObject *Sender)
{
	if(ComboBox1->ItemIndex == 0) analiseAC.escala = LIN;
	else if (ComboBox1->ItemIndex == 1)  analiseAC.escala = DEC;
	else if (ComboBox1->ItemIndex == 2)  analiseAC.escala = OCT;
}
//---------------------------------------------------------------------------



