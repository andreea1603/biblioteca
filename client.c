#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

extern int errno;
int port;
void descarcare(int sd);

int main (int argc, char *argv[])
{
  
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  char msg[300];	

  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  port = atoi (argv[2]);

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);
  
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }
  int parola_corecta=0, pasi=0;
  bzero (msg, 300);

   printf("\033[1;35m");
  printf ("Introduceti un nume: ");
  fflush (stdout);
  read (0, msg, 300);
    printf("\033[0;35m");
  //trimitere username
  if (write (sd, msg, 300) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
 // printf("am trimis username \n");
  //validare username
  char p[300];
  while(parola_corecta==0 )
  { 
    char parola[300], valid[300];
    pasi++;
    bzero(p,300);
    bzero(parola,300);
    bzero(valid,300);
    if(read(sd, p, 300)<=0)
    {
        perror("eroare validare");
    }
  
    printf("%s", p);
    fflush (stdout);
    //trimitere parola
    if(read(0, parola,300)< 0)
    {
      perror("eroare la citire parola");
    }
    //printf("Am trimis parola la server\n");
    if(write(sd, parola, 300)<0)
    {
      perror("eroare la trimitere parola");
    }
    // citire daca parola e buna
    //printf("Citesc validare\n");
    if(read(sd, valid,300 )<0)
    {
      perror("eroare la citire validare");
    }
    //printf("v:$ %s", valid);
    if(strstr(valid,"gresit")==0)
    {
        parola_corecta=1;
    }
  
  }
  int prima_op=0;
  //MESAJUL DE WELCOME + CITIRE OPTIUNI 
  if (read (sd, msg, 300) < 0)
    {
      perror ("[client]Eroare la read() de la server.\n");
      return errno;
    }
   printf("\033[1;33m");
  /* afisam mesajul primit */
  printf (" \n    %s\n", msg);

  // SCRIEREA OPTIUNILOR ALESE 
  char optiuni[300], rezultat[300];
  while(1)
  { 
    int m;
    bzero (optiuni, 300);
    prima_op++;
    //citirea optiunii alease
  
    if(read (0, optiuni, 300)<=0)
    {
      perror ("[client]Eroare la citire optiune () spre server.\n");
      return errno;
    }
      optiuni[strlen(optiuni)-1]=NULL;

    //trimiterea optiunii alese

    if (write (sd, optiuni, 300) <= 0)  
    {
      perror ("[client]Eroare la write optiune() spre server.\n");
      return errno;
    }

    if(strcmp(optiuni, "quit")==0)
    {       
          printf("\033[0;32m");
          printf("\nDeconectat!\n");
          close (sd);
          return 0;
    }
    //citirea rezultatului
    else
        { 
        if(strcmp(optiuni, "recomandare")==0)
          {
              printf("\033[0;35m");
              char carti_recomandate[300], autori[300];
              for(int j=0 ; j < 10; j++)
              {
                bzero(carti_recomandate,300);
                bzero(autori,300);
                if(read(sd, carti_recomandate, 300 )<0)
                {
                  perror("eroare citire carti recomandate");
                }
                if(read(sd, autori, 300 )<0)
                {
                  perror("eroare citire carti recomandate");
                }
                else{
                  printf("\033[0;35m");
                  printf("\n%d: %s , %s \n", j+1, carti_recomandate, autori);
                }
              }
          }
        else
            if(strcmp(optiuni, "evaluare")==0)
            {
              int d=0;
              printf("\033[0;34m");
              char msg_sv1[300], msg_sv2[300], msg_sv3[300], msg_rec1[300];
              char nume_carteev[300], numeautor[300],  msg_rec2[300], msg_rec3[300];
              float nota_primita;
              bzero(msg_sv1,300); bzero(msg_sv2,300); bzero(msg_rec1,300);
              bzero(msg_sv3,300);
              bzero(msg_rec2,300);bzero(nume_carteev,300);bzero(numeautor,300);
              //citire carte
                if(read(sd, msg_sv1, 300)<0)
                    { perror("citire eval1");}
                printf("\n%s\n", msg_sv1);
                printf("\033[0;37m");
                if(read(0,nume_carteev,300 )<0)
                    { perror("citire carte");}
                if(write(sd, nume_carteev, 300)<0)
                    { perror("citire carte");}
              //citire autor
              printf("\033[0;34m");
                  if(read(sd, msg_sv2, 300)<0)
                      { perror("citire eval2");}
                      printf("\033[0;35m");
                  printf("%s\n", msg_sv2);
                  printf("\033[0;37m");
                  if(read(0,numeautor,300 )<0)
                      { perror("citire autor");}
                      printf("\033[0;35m");
                  if(write(sd, numeautor, 300)<0)
                      { perror("citire autor");}
              //citire nota
              printf("\033[0;34m");
              if(read(sd, msg_sv3, 300)<0)
                  { perror("citire eval2");}
              printf("%s\n", msg_sv3);
              scanf("%f", &nota_primita);
              //printf("%f\n", nota_primita);
              if(write(sd, &nota_primita, sizeof(float))<0)
                  { perror("citire autor");}
                bzero(msg_sv1, 300);
              if(read(sd, msg_sv1, 300)<0)
              {
                perror("eroare primit");
              }
              printf("%s\n", msg_sv1);
            }
            else
                if(strcmp(optiuni, "cautare")==0)
                {
                  printf("\033[0;36m");
                    int op;
                    bzero(rezultat, 300);
                    if( read(sd, rezultat, 300)<0)
                    {
                      perror ("[client]Eroare la read() de la server.\n");
                      return errno;
                    }
                    else
                    {
                        printf("\n   %s\n", rezultat);
                    }
                    //citesc optiunea pe care o doresc
                   int  n=scanf("%d", &op);
                    if(n==0)
                    {
                      op=1;
                    }
                    if(write(sd, &op, sizeof(int))<0)
                        perror("citire autor"); 
                    //inserare valori
                    //citeste ce ar trebui sa citeasca y/n
                    char values1[12];
                    if(read(sd, values1, 12)<0)
                        perror("eroare citire ");
                   // printf("%s\n", values1);
                  // 1-gen 2- autor 3- numele cartii 4-an 5-isbn 6-rating
                  if(values1[1]=='y') //gen
                  {
                          printf("\033[0;34m");
                          bzero(rezultat, 300);
                          char gen_cautat[100];

                          if( read(sd, rezultat, 300)<0)
                          {
                            perror ("[client]Eroare la read() de la server.\n");
                            return errno;
                          }
                          else
                          {
                              printf("%s\n", rezultat);
                          }  
                          //citesc genul pe care trebuie sa-l caut
                          bzero(gen_cautat,100);
                          if(read(0, gen_cautat,100 )<0) perror("eroare la citire gen");
                          //il trimit
                          if(write(sd, gen_cautat, 100)<0) perror("eroare trimitere gen");
                  }
                  if(values1[2]=='y') //daca am nevoie si de autor
                  {

                          printf("\033[0;34m");
                          bzero(rezultat, 300);
                          char autor_cautat[100];

                          if( read(sd, rezultat, 300)<0)
                          {
                            perror ("[client]Eroare la read() de la server.\n");
                            return errno;
                          }
                          else
                          {
                              printf("%s\n", rezultat);
                          }  
                          //citesc genul pe care trebuie sa-l caut
                          bzero(autor_cautat,100);
                          if(read(0, autor_cautat,100 )<0) perror("eroare la citire gen");
                          //il trimit
                          if(write(sd, autor_cautat, 100)<0) perror("eroare trimitere gen");
                  }
                  if(values1[3]=='y') //daca am nevoie si de numele cartii
                  {

                          printf("\033[0;34m");
                          bzero(rezultat, 300);
                          char carte_cautata[100];

                          if( read(sd, rezultat, 300)<0)
                          {
                            perror ("[client]Eroare la read() de la server.\n");
                            return errno;
                          }
                          else
                          {
                              printf("%s\n", rezultat);
                          }  
                          //citesc genul pe care trebuie sa-l caut
                          bzero(carte_cautata,100);
                          if(read(0, carte_cautata,100 )<0) perror("eroare la citire gen");
                          //il trimit
                          if(write(sd, carte_cautata, 100)<0) perror("eroare trimitere gen");
                  }
                  if(values1[4]=='y') //anul
                  {
                            printf("\033[0;36m");
                            bzero(rezultat, 300);
                            if( read(sd, rezultat, 300)<0)
                            {
                              perror ("[client]Eroare la read() de la server.\n");
                              return errno;
                            }
                            else
                            {
                              printf("\033[0;34m");
                                printf(" %s\n", rezultat);
                            }  
                            int var;
                            scanf("%d", &var);
                            if(write(sd, &var, sizeof(int))<0) perror("eroare trimitere gen");

                    }
                  if(values1[5]=='y') //isbn
                  {
                            printf("\033[0;36m");
                            bzero(rezultat, 300);
                            if( read(sd, rezultat, 300)<0)
                            {
                              perror ("[client]Eroare la read() de la server.\n");
                              return errno;
                            }
                            else
                            {
                              printf("\033[0;34m");
                                printf(" %s\n", rezultat);
                            }  
                            //citesc genul/autorul/cartea pe care trebuie sa-l caut
                            int var;
                            scanf("%d", &var);
                            if(write(sd, &var, sizeof(int))<0) perror("eroare trimitere gen");

                    }
                    if(values1[6]=='y') //rating
                  {
                            printf("\033[0;36m");
                            bzero(rezultat, 300);
                            if( read(sd, rezultat, 300)<0)
                            {
                              perror ("[client]Eroare la read() de la server.\n");
                              return errno;
                            }
                            else
                            {
                              printf("\033[0;34m");
                                printf(" %s\n", rezultat);
                            }  
                            //citesc genul/autorul/cartea pe care trebuie sa-l caut
                            int var;
                            scanf("%d", &var);
                            if(write(sd, &var, sizeof(int))<0) perror("eroare trimitere gen");

                    }
                  
                //acum trebuie doar sa astept rezultatul
                    char carti_cautate[10000];
                    printf("\e[0;33m");
                    if(read(sd, carti_cautate, 10000)<0)
                    {
                      perror("eroare primire carti");
                    }
                    else
                      printf("%s\n", carti_cautate);

                }
                     
              else
                if(strcmp(optiuni, "descarcare")==0)
                {  
                  printf("\033[0;32m");
                    descarcare(sd);
                }
                else
                {    
                      bzero(rezultat, 300);
                      if( read(sd, rezultat, 300)<0)
                      {
                        perror ("[client]Eroare la read() de la server.\n");
                        return errno;
                      }
                      else
                      {
                        printf("\033[0;31m");
                          printf("%s\n", rezultat);
                      }
                  }
        }
     printf("\033[0;33m");
     int n;
     n=strlen(msg)-110;
    printf("\n%s\n", msg+n);
  }
  close (sd);
  
}
void descarcare(int sd)
{
  int isbn;
  char msg[100];
  bzero(msg,100);
  int valid;
  //citesc mesajul de "intampinare"
  if(read(sd, msg, 100)<0)
  {
    perror("eroare read");
  }
  printf("%s\n", msg);
    //citirea isbn-ului cartii dorite
    scanf("%d", &isbn);
    //il trimit serverului
    if(write(sd, &isbn, sizeof(int))<0)
    {
      perror("erpare la tr ibs");
    }
  //crearea unui fisier
  FILE *fp;
  char filename[100], mesajer[100];

  if(read(sd, &valid, sizeof(int))<0)
    {
      perror("eroare la citire valid");

    }
  
    else
    {
      if(valid==0)
      {
        bzero(mesajer,100);
        if(read(sd, mesajer, 100)<0)
        {
          perror("eroare mesaj inv");
        }
        else
          {printf("%s\n", mesajer);}
      }
      else{
        sprintf(filename, "%d",isbn);
        fp= fopen(filename, "a+");
        if(fp==NULL)
        {
          perror("eroare desc fisier");
        }
        char carte[1024];
        int n=1;
        while(n>0)
        {
            if(read(sd, carte, sizeof(carte))<=0)
                {
                    perror("eroare citire");
                 }
            if(strcmp(carte, "gata")==0)
            {
              break;
            }
            fprintf(fp, "%s", carte);
            bzero(carte, 1024);
        }
        printf("Descarcare cu succes!\n");
      }
    }
  }
