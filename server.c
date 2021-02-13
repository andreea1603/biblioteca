#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sqlite3.h>
#define PORT 2024
void handler(int sig);
void crearelib(sqlite3* db);
void recommendation(sqlite3* db, int client, char aux1[100]);
void evaluation(sqlite3* db, int client, char aux1[100]);
void search(sqlite3* db, int client, char msgrasp[300], char aux1[200]);
void search2(sqlite3* db, int client, char msgrasp[300], char aux1[200]);
void descarcare(sqlite3* db, int client, char aux1[200]);
extern int errno;
char* err;
int main ()
{
    sqlite3* db;
    sqlite3_stmt* stmt;
    sqlite3_stmt* stmt1;
    int a1,a2,k;

    const unsigned char* myname;
  //creare tabel
    sqlite3_open("myDb1.db", &db);
    crearelib(db);
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  int status;
  char msg[300];		
  char msgrasp[300]=" ";       
  int sd;		
  int nsd;
  struct sockaddr_in newAddr;
  socklen_t addr_size;
  pid_t pid;
  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }
  int optval=1;
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));

    server.sin_family = AF_INET;	
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);
  
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  if (listen (sd, 150) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }

  while (1)
    {
      int client;
      int length = sizeof (from);
      int k=0;

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      client = accept (sd, (struct sockaddr *) &from, &length);

      /* eroare la acceptarea conexiunii de la un client */
      if (client < 0)
	    {
	          perror ("[server]Eroare la accept().\n");
	          continue;
	    }

      if((pid=fork())==-1)
      {
            perror("eroare fork");}
      else
      {
          k++;
          if(pid==0)
          { 
               close(sd);
                /* s-a realizat conexiunea, se astepta mesajul */
                bzero (msg, 300);
                printf ("[server]Asteptam mesajul...\n");
                fflush (stdout);
      
                /* citirea mesajului */
                if (read (client, msg, 300) <= 0)
	              {
	                  printf("Client deconectat\n");
	                  close (client);	                                                      return 0;		
              	}

                    //cautarea clientului in baza de date

                printf ("[server]Mesajul a fost receptionat...%s\n", msg);
                   char sel[300],aux1[300], trparola[300], parolaa[300], verificare_parola[300];
                   const unsigned char* myname;
                    strcpy(sel, "select username, cautari from client where username='");
                    strcpy(aux1,msg);
                    aux1[strlen(aux1)-1]=NULL;
                    strcat(sel, aux1);
                    strcat(sel, "'");
                    sqlite3_prepare_v2(db, sel, -1, &stmt, 0);
                    int k1=0, a2;
                    while(sqlite3_step(stmt)!=SQLITE_DONE)
                    {
                        k1++;
                        a2=sqlite3_column_int(stmt,1);
                        myname=sqlite3_column_text(stmt, 0);
                        printf("Numele %s, %d\n", myname, a2);
                    }
                    // daca nu exista clientul in baza de date, atunci se va adauga in baza de date noul username
                    if(k1>0){   //verificare parola
                              int parola_buna=0, incercari=0;;
                              while(parola_buna==0)
                              {
                                  const unsigned char* mypsw;
                                  bzero(trparola, 300);
                                  incercari++;
                                  bzero(parolaa,300); bzero(verificare_parola,300);
                                  strcpy(trparola, "Introduceti parola : ");
                                  //trimitere mesaj
                                  if(write(client, trparola, 300)<=0)
                                  {
                                    perror("eroare trimitere parola");
                                  }
                                  //citire parola
                                  if(read(client, parolaa, 300)<=0)
                                  {
                                    printf("Client deconectat\n");
                                     close (client);	 return 0;	
                                  }
                                  parolaa[strlen(parolaa)-1]=NULL;
                                  strcpy(verificare_parola, "select parola from client where username='");
                                  strcat(verificare_parola, aux1);
                                  strcat(verificare_parola, "' and parola='");
                                  strcat(verificare_parola, parolaa);
                                  strcat(verificare_parola, "'");
                                  int count=0;
                                  sqlite3_prepare_v2(db, verificare_parola, -1, &stmt1, 0);
                                  while(sqlite3_step(stmt1)!=SQLITE_DONE)
                                  {   
                                      mypsw=sqlite3_column_text(stmt1, 0);
                                      count++;
                                  }
                                  char trimi_valid[300];
                                  if(count==1)
                                  {
                                  parola_buna=1;
                                  strcpy(trimi_valid, "BINE AI REVENIT!\n");
                                  }
                                  else
                                  {
                                    strcpy(trimi_valid, "Parola gresita!\n");
                                  }
                                  printf("Trimit validare\n");
                                  if(write(client, trimi_valid, 300)<=0)
                                  {
                                    perror("eroare la trimitere validare");
                                  }
                              }
                               // printf("\nBINE AI REVENIT!\n"); 
                            }
                    else {    //adaugare parola
                              bzero(trparola, 300);
                              bzero(parolaa,300);
                              strcpy(trparola, "Cont nou! Introduceti noua parola: ");
                              if(write(client, trparola, 300)<=0)
                              {
                                perror("eroare trimitere parola");
                              }
                              if(read(client, parolaa, 300)<=0)
                              {
                                printf("Client deconectat\n");
                                close (client);	 return 0;	
                              }
                             // printf("\nBINE AI VENIT!\n");
                              char query[300], aux[300];
                              strcpy(aux,msg);
                              aux[strlen(aux)-1]=NULL;
                              parolaa[strlen(parolaa)-1]=NULL;
                              strcpy(query,"insert into client VALUES ('");
                              strcat(query, aux); strcat(query, "','");
                              strcat(query, parolaa); 
                              strcat(query,"', 0)");
                              //printf("\n%s\n", query);
                              int rc=sqlite3_exec(db, query,NULL, NULL, &err);
                              if(rc!=SQLITE_OK)
                              {
                                      printf("eroare la adaugare");
                                      perror("eroare: ");
                              }
                              char mesaj_valid[300];
                              bzero(mesaj_valid,300);
                              strcpy(mesaj_valid, "Bine ai venit!");
                              if(write(client, mesaj_valid, 300)<0)
                                  {
                                    perror("eroare la trimitere validare");
                                  }
                          }
      
                 /*pregatim mesajul de raspuns */
                bzero(msgrasp,300);
                if(k1>0) {  strcat(msgrasp, "Bine ai revenit,");}
                else     {  strcat(msgrasp,"Bine ai venit, ");          }
                strcat(msgrasp,msg);
                strcat(msgrasp,"\n  Alege optiunea preferata:\n");
                strcat(msgrasp,"    (1) cautare\n    (2) evaluare\n    (3) descarcare\n    (4) recomandare\n    (5) quit");
                     
                 // trimitem mesajul de welcome clientului 

                if (write (client, msgrasp, 300) <= 0)
                {
                  perror ("[server]Eroare la write() catre client.\n");
                  continue;	
                }
                else
                      printf ("[server]Mesajul a fost trasmis cu succes.\n");
                
                
                while(1)
                {
                    fflush(stdout);
                    bzero (msg, 300);
                    // se citeste optiunea clientului 
                    if (read (client, msg, 300) <= 0)
	                  {
	                    //perror ("[server]Eroare la read() de la client.\n");
	                    close (client);	
                        return 0;
              	    }
                   // printf("\nAm primit optiunea: %s\n", msg);
                    //daca este optiunea de inchidere, atunci inchidem conexiunea cu clientul
                    if(strcmp(msg, "quit")==0)
                    {
                        printf("\nClientul a iesit\n");
                        close (client);
                        return 0;
                    }  
                    else
                        if(strcmp(msg, "recomandare")==0)
                        {    
                                    recommendation(db, client, aux1);
                        }
                        else
                              if(strcmp(msg, "evaluare")==0)
                              {     
                                    evaluation(db, client, aux1);
                              }
                              else
                                    if(strcmp(msg, "cautare")==0)
                                    {
                                          search2(db, client, msgrasp, aux1);
            
                                    }                                                                                                                                                                     
                                    else
                                          if(strcmp(msg, "descarcare")==0)
                                          {
                                               
                                                descarcare(db, client, aux1);
                                          }
                                          else
                                          {   
                                                bzero(msgrasp,300);
                                                strcpy(msgrasp," Optiune invalida, incercati din nou!");
                                                strcat(msgrasp,msg);
                                                if (write (client, msgrasp, 300) <= 0)
                                                {
                                                perror ("[server]Eroare la write() catre client.\n");
                                                continue;		/* continuam sa ascultam */
                                                }
                                                else
                                                printf ("[server]Mesajul a fost trasmis cu succes.\n");
                                          }   
      
                   }
          }	
            else
            {
              signal(SIGCHLD, handler);
               close(client);
            }
    
      }	
    }
}
//functia de descarcare;
void descarcare(sqlite3* db, int client, char aux1[200])
{
      //citirea isnb-ului cartii dorite
      int isbn;
      char msg[100];
      bzero(msg, 100);
      strcpy(msg, "Introduceti ISBN-ul cartii dorite:");

      if(write(client, msg, 100)<0)
      {
            perror("eroare write");
      }

      if(read(client, &isbn, sizeof(int))<0)
      {
            perror("erpoare isnb");
      }
      //printf("%d\n", isbn);
      FILE *fp;
      char filename[100], mesaj[100];
      bzero(filename, 100);
      sprintf(filename,"mybooks/%d", isbn);
      int valid=0;
      fp=fopen(filename, "r");
      if(fp==NULL)
      {     bzero(mesaj,100);
            strcpy(mesaj, "Nu exista o carte cu acest ISBN");
            if(write(client,&valid,sizeof(int))<0)
            {
                  perror("nu exista ");
            }
            else
            {
                  if(write(client, mesaj, 100)<0)
                  {
                        perror("eroare trimit mesg");
                  }
            }
      }
      else{
            char trimit[1024]={0};
            
            valid=1;
            if(write(client, &valid, sizeof(int))<0)
            {
                  perror("eroare tr valid");
            }
            else
            {
                  while(fgets(trimit, 1024, fp)!=NULL)
                  {
                     
                        if(write(client, trimit, sizeof(trimit))<0)
                        {
                              perror("eroare trimitere carte");
                              exit(1);
                        }
                  }
                  bzero(trimit, 1024);
                  strcpy(trimit,"gata");                  
                        if(write(client, trimit, sizeof(trimit))<0)
                        {
                              perror("eroare trimitere carte");
                              exit(1);
                        }
                  bzero(trimit, 1024);
            }
            const unsigned char* cart, *gen;
            const unsigned char* a2, *subgen;
            char sel[3000], rezultat[1000]; 
            sqlite3_stmt* stmt;
            bzero(sel,300);
            sprintf(sel, "select carte, autor, gen, subgen from librarie where ISBN=%d", isbn);

            sqlite3_prepare_v2(db, sel, -1, &stmt, 0);
      
            bzero(rezultat,1000);


            while(sqlite3_step(stmt)!=SQLITE_DONE)
            {
                  cart=sqlite3_column_text(stmt, 0);
                  gen=sqlite3_column_text(stmt, 1);
                  a2=sqlite3_column_text(stmt, 2);
                  subgen=sqlite3_column_text(stmt, 3);
                  char query[300];
                  bzero(query,300);
                  //cautdesc(username VARCHAR2(300), numecarte VARCHAR2(300), 
                  //autor VARCHAR2(300), gen VARCHAR2(300), cautare INT, descarcare INT)"
                  int rc;
                  sprintf(query,"insert into cautdesc VALUES ('%s', '%s' , '%s' , '%s', '%s', 0, 1 )", aux1, cart, gen, a2, subgen);
            
                  rc=sqlite3_exec(db, query,NULL, NULL, &err);
                  if(rc!=SQLITE_OK)
                  {
                        printf("eroare la adaugare autor4");  
                        perror("eroare: ");                         }
            }
            
      }


}
int frecv[10];
void search2(sqlite3 *db, int client, char msgrasp[300], char aux1[200])
{
      int op;
      for(int i=0;i <=10; i++)
            frecv[i]=0;

      bzero(msgrasp,300);
      strcpy(msgrasp, "Alegeti cifra corespunzatoare criterilui dorit : \n");
      strcat(msgrasp, "1) dupa gen \n2) dupa autor \n3) dupa numele cartii\n");
      strcat(msgrasp, "4) dupa anul aparitiei\n5) dupa ISBN \n6) dupa rating");
 
      if (write (client, msgrasp, 300) < 0)
      {
            perror("[server]Eroare la write() catre client");	
      }
      else
            printf ("[server]Mesajul a fost trasmis cu succes.\n");
      //citesc numarul ales
      if(read(client, &op, sizeof(int))<0)
            perror("citire ");
      while(op!=0)
      {
            if(op%10>6)
            {
                  printf("eroare numar incorect");
            }
            else
            {
                  frecv[op%10]++;
                  op=op/10;
            }
      }
      char trimit[10];
      bzero(trimit, 10);
      for(int i=0; i<= 6; i++)
            if(frecv[i]>=1)
                  strcat(trimit,"y");
            else
                  strcat(trimit, "n");
      //printf("%s\n", trimit);
      
      if(write(client, trimit, 11)<0)
      {
            perror("eroare trimitere ");
      }
      //bzero(trimit, 10);
      // 1-gen 2- autor 3- numele cartii 4-an 5-isbn 6-rating
      char genul[100], autorul[100], nume_carte[100], semi_query[10000];
      int an, isbn, rating;
      //trimit[1]
      int am_inserat=0;
      bzero(semi_query,10000);
      if(trimit[1]=='y') //trebuie sa citesc genul
      {
            bzero(genul,100);
            strcpy(genul, "Introduceti genul cautat: ");
            //trimit mesajul
         
            if (write (client, genul, 100) <= 0)
                  perror ("[server]Eroare la write() catre client.\n");
            else
                  printf ("[server]Mesajul a fost trasmis cu succes.\n");
            //citesc genul dorit
            bzero(genul,100);
            if(read(client, genul, 100 )<0) perror("eroare citire gen");
    
            genul[strlen(genul)-1]=NULL;
            sprintf(semi_query, "UPPER(gen)=UPPER('%s') and ", genul);

      }
      if(trimit[2]=='y')      //numele autorului
      {
                  bzero(autorul,100);
            strcpy(autorul, "Introduceti autorul cautat: ");
            //trimit mesajul
 
            if (write (client,autorul, 100) <= 0)
                  perror ("[server]Eroare la write() catre client.\n");
            else
                  printf ("[server]Mesajul a fost trasmis cu succes.\n");
            //citesc autorul
            bzero(autorul,100);
            if(read(client, autorul, 100 )<0) perror("eroare citire gen");
            autorul[strlen(autorul)-1]=NULL;
            sprintf(semi_query+strlen(semi_query), "UPPER(autor)=UPPER('%s') and ", autorul);

      }
            if(trimit[3]=='y')      //numele cartii 
      {
            bzero(nume_carte,100);
            strcpy(nume_carte, "Introduceti cartea cautata: ");
            //trimit mesajul

            if (write (client,nume_carte, 100) <= 0)
                  perror ("[server]Eroare la write() catre client.\n");
            else
                  printf ("[server]Mesajul a fost trasmis cu succes.\n");
            //citesc autorl dorit
            bzero(nume_carte,100);
            if(read(client, nume_carte, 100 )<0) perror("eroare citire gen");
            //printf("%s\n", nume_carte);
            nume_carte[strlen(nume_carte)-1]=NULL;
            strcat(semi_query, "UPPER(carte) like UPPER('%");
            strcat(semi_query,nume_carte);
            strcat(semi_query, "%') and ");

      }
            if(trimit[4]=='y')//anul
      {

            int an_primit;
            bzero(msgrasp,300);
            strcpy(msgrasp, "Introduceti anul aparitiei: ");
            //trimit mesajul
            if (write (client, msgrasp, 300) <= 0)
                  perror ("[server]Eroare la write() catre client.\n");
            else
                  printf ("[server]Mesajul a fost trasmis cu succes.\n");
            //citesc autorul dorit
            bzero(msgrasp,300);

            if(read(client, &an_primit, 300 )<0) perror("eroare citire an");
            an=an_primit;
            sprintf(semi_query+strlen(semi_query), "an_aparitie=%d and ", an);

      }
            if(trimit[5]=='y')
      {
            bzero(msgrasp,300);
            strcpy(msgrasp, "Introduceti ISBN: ");
            //trimit mesajul
            if (write (client, msgrasp, 300) <= 0)
                  perror ("[server]Eroare la write() catre client.\n");
            else
                  printf ("[server]Mesajul a fost trasmis cu succes.\n");
            //citesc
            bzero(msgrasp,300);

            if(read(client, &isbn, 300 )<0) perror("eroare citire an");
             sprintf(semi_query+strlen(semi_query), "ISBN=%d and ", isbn);

      }
            if(trimit[6]=='y')      //rating
      {

            bzero(msgrasp,300);
            strcpy(msgrasp, "Introduceti rating: ");
            //trimit mesajul
            if (write (client, msgrasp, 300) <= 0)
                  perror ("[server]Eroare la write() catre client.\n");
            else
                  printf ("[server]Mesajul a fost trasmis cu succes.\n");
            //citesc 
            bzero(msgrasp,300);

            if(read(client, &rating, 300 )<0) perror("eroare citire an");
             sprintf(semi_query+strlen(semi_query), "(rating>%d-1 and rating<%d+1) and ", rating, rating);

      }

      
      //printf("%s, %s, %s, %d, %d, %d \n", autorul, genul, nume_carte, an, isbn, rating);
      char sel[1000];
      int k=0;
      bzero(sel,1000);
      strcpy(sel, "select carte, autor, an_aparitie, ISBN, rating, gen, subgen from librarie where ");
      strcat(sel, semi_query);
      sel[strlen(sel)-4]=NULL;
      //printf("%s\n", sel);
      sqlite3_stmt* stmt;
      const unsigned char* cart, *aut;
      const unsigned char* a2, *a3;
      int isbn1, an1;
      float rating1;
      int nr_in=0;
      char rezultat[10000], rezfin[10000], comp1[100], comp2[100];
      bzero(rezultat, 10000);
      bzero(rezfin,10000);
      sqlite3_prepare_v2(db, sel, -1, &stmt, 0);

      while(sqlite3_step(stmt)!=SQLITE_DONE)
            {
                  nr_in++;
                  cart=sqlite3_column_text(stmt, 0);
                  aut=sqlite3_column_text(stmt, 1);
                  an1=sqlite3_column_int(stmt, 2);
                  isbn1=sqlite3_column_int(stmt, 3);
                  rating1=sqlite3_column_double(stmt, 4);
                  a2=sqlite3_column_text(stmt, 5);
                  a3=sqlite3_column_text(stmt, 6);
                  if(nr_in==1)
                        strcpy(comp1, a2);
                  sprintf(rezultat, "     Numele cartii: %s, nume autor: %s, anul aparitiei: %d, ISBN: %d, rating: %f\n", cart, aut, an1, isbn1, rating1);
                  strcat(rezfin, rezultat);
                  char queryf[300];
                  bzero(queryf,300);
                  //cautdesc(username VARCHAR2(300), numecarte VARCHAR2(300), 
                  //autor VARCHAR2(300), gen VARCHAR2(300), cautare INT, descarcare INT)"
                  strcpy(comp2, a2);

                  if((nr_in!=1 && strcmp(comp1, comp2)!=0) || nr_in==1)
                  {     int rc;
                        sprintf(queryf,"insert into cautdesc VALUES ('%s', NULL , NULL, '%s', '%s',  1, 0 )", aux1, a2, a3);
                        rc=sqlite3_exec(db, queryf,NULL, NULL, &err);
                        if(rc!=SQLITE_OK)
                        {
                              printf("eroare la adaugare autor4");  
                              perror("eroare: ");                         }
                       // printf("%s\n", queryf);
                  }
            }
      if(nr_in==0)
            strcpy(rezfin,"Nu exista astfel de carti!");
      if(write(client, rezfin, 10000)<0)
      {
            perror("eroare la trimitere cautare");
      }      

}

//functia de evaluare
void evaluation(sqlite3* db, int client, char aux1[100])
{
      float nota_primita;
      char msg_cl1[300], msg_cl2[300], msg_cl3[300], msg_rec1[300], msg_rec2[300];
      bzero(msg_cl1,300); bzero(msg_cl2,300); bzero(msg_rec1,300);
      bzero(msg_rec2,300);
      strcpy(msg_cl1,"Introduceti numele cartii");
      if(write(client, msg_cl1, 300)<0)
      {
            perror("eroare trimitere mesaj evluare1");
      }
      if(read(client, msg_rec1, 300)<0)
      {
            perror("citire aut");
      }
      strcpy(msg_cl2,"Introduceti numele autorului");
      if(write(client, msg_cl2, 300)<0)
      {
            perror("eroare trimitere mesaj evluare1");
      }
      if(read(client, msg_rec2, 300)<0)
      {
            perror("citire aut");
      }
      strcpy(msg_cl3, "Introduceti o nota de la 1 la 10");
      if(write(client, msg_cl3, 300)<0)
      {
            perror("eroare trimitere mesaj evluare1");
      }
      if(read(client, &nota_primita, sizeof(float))<0)
      {
            perror("citire ");
      }
      msg_rec1[strlen(msg_rec1)-1]=NULL;
      msg_rec2[strlen(msg_rec2)-1]=NULL;
    //  printf("%sQ \n %sQ \n %f\n", msg_rec1, msg_rec2, nota_primita);
      //verificare existenta carte primita 
      const unsigned char* cart, *gen;
      const unsigned char* a2;
      char sel[3000], rezultat[1000], rezfin[1000], genn[100];   
      sqlite3_stmt* stmt;
      sprintf(sel, "select gen from librarie where UPPER(carte)=UPPER('%s') and UPPER(autor)=UPPER('%s')", msg_rec1, msg_rec2);
      sqlite3_prepare_v2(db, sel, -1, &stmt, 0);
      int k=0, m;
      while(sqlite3_step(stmt)!=SQLITE_DONE )
      {    
            k++;
            gen=sqlite3_column_text(stmt, 0);
            strcpy(genn, gen);
      }
      if(k==0)
      {
            strcpy(rezfin, "Nu exista aceasta carte!");
      }
      else
      {
            strcpy(rezfin, "Evaluare realizata cu succes!");
             char query[1000];
             bzero(query,1000);

             int rc;
             if(nota_primita>=8)
             {
                  printf("%s\n", genn);
                   sprintf(query,"insert into cautdesc VALUES ('%s', '%s' , '%s' , '%s', NULL ,  1,  1 )", aux1, msg_rec1, msg_rec2, genn);
                   printf("%s\n", query);
                   rc=sqlite3_exec(db, query,NULL, NULL, &err);
                   if(rc!=SQLITE_OK)
                   {
                         printf("eroare la adaugare autor4");  
                         perror("eroare: ");                         }

             }


      }
  //    printf("%s\n", rezfin);



      ///////////////
      int req;
      char update[200], update2[200];
      bzero(update, 200); bzero(update2,200);
      //update numar de utilizatori care au evaluat cartea
      strcpy(update, "UPDATE librarie SET utilizrating=utilizrating+1 WHERE UPPER(carte) =UPPER('");
      strcat(update, msg_rec1); strcat(update, "') and UPPER(autor) =UPPER('");
      strcat(update, msg_rec2); strcat (update, "')");
     // printf("%s\n", update);
      req=sqlite3_exec(db, update, NULL, NULL, &err);
      if(req!=SQLITE_OK)
      {
            perror("eroare la update 1");
      }
      //update nota primita
      char n[300]; 
      bzero(n,300);
      sprintf(n, "%f", nota_primita);
      strcpy(update2, "UPDATE librarie SET rating= (rating*(utilizrating - 1)+");
      strcat(update2, n);
      strcat(update2, ") / utilizrating WHERE UPPER(carte) = UPPER('");
      strcat(update2, msg_rec1); strcat(update2, "') and UPPER(autor) =UPPER('");
      strcat(update2, msg_rec2); strcat (update2, "')");

      req=sqlite3_exec(db, update2, NULL, NULL, &err);
      if(req!=SQLITE_OK)
      {
            perror("eroare la update 2");
      }
      if(write(client, rezfin, 100)<0)
      {
            perror("eroare la trimitere");
      }
}		
//functia de recomandare
int la_fel[10000];
void recommendation(sqlite3* db,int client, char aux1[100])
{
      const unsigned char* myname;
      const unsigned char* a2;
      const unsigned char *cname, *caut;
      char sel[3000], sel2[300]; int nr_in=0, prim_doi=0, nr_carti=0;
     // printf("Nume utilizator : %s\n", aux1);
      sqlite3_stmt* stmt, *stmt2, *stmt3, *stmt4;
      strcpy(sel, "select gen, count(gen) from cautdesc where username='");
      strcat(sel, aux1);
      strcat(sel, "' group by gen order by count(gen) desc ");
      sqlite3_prepare_v2(db, sel, -1, &stmt, 0);
      //afisez primele  carti din preferatele lui, apoi urmatoarele din preferatele celorlalti ;
      for(int i=0; i<10000; i++)
            la_fel[i]=0;
      int add_isbn=0;
      while(sqlite3_step(stmt)!=SQLITE_DONE)
      {
      nr_in++;
      //printf("nr%d \n", nr_in);
      a2=sqlite3_column_text(stmt,1);
      myname=sqlite3_column_text(stmt, 0);
      //printf("Numele %s, %s\n", myname, a2);
            if(nr_in<10)
            {
                  prim_doi=1;
                  bzero(sel2,300);
                  float rating;
                  //username VARCHAR2(300), numecarte VARCHAR2(300), autor VARCHAR2(300), gen VARCHAR2(300), cautare INT, descarcare INT)", NULL, NUL
                  sprintf(sel2, "select carte, autor, ISBN, rating from librarie where UPPER(gen) LIKE UPPER('%s') order by rating desc", myname);
                  sqlite3_prepare_v2(db, sel2, -1, &stmt2, 0);
                  while(sqlite3_step(stmt2)!=SQLITE_DONE)
                  {
                        
                        cname=sqlite3_column_text(stmt2, 0);
                        caut=sqlite3_column_text(stmt2,1);
                        add_isbn=sqlite3_column_int(stmt2, 2);
                        rating=sqlite3_column_double(stmt2, 3);
                        //verificam daca nu a descarcat-o inca
                        char sel3[100];
                        sprintf(sel3, "select 2 from cautdesc where username='%s' and UPPER(numecarte)=UPPER('%s') and descarcare=1", aux1, cname);
                        //                              printf("%s\n", cname);
                        //printf("%s\n", sel3);
                        sqlite3_prepare_v2(db, sel3, -1, &stmt3, 0); int read_book=0;
                        while(sqlite3_step(stmt3)!=SQLITE_DONE)
                        {
                              read_book++;
                              printf("%s\n", cname);
 
                        }
                        if(read_book==0)
                        {
                              nr_carti++;
                              la_fel[add_isbn]++;
                             if(write(client, cname, 300)<0)
                              {
                                    perror("eroare la trimitere carte recomandata");
                              }
                              if(write(client, caut, 300)<0)
                              {
                                    perror("eroare la trimitere carte recomandata");
                              }
                              printf("%s%f\n", cname, rating);
                        }
                  }
            }

      }
      if(nr_carti<10 && nr_in!=0)
      {
                        char sel3[300], sel4[300];
                        float rating;
                        sprintf(sel3, "select carte, autor, ISBN, rating from librarie order by rating desc");
                        sqlite3_prepare_v2(db, sel3, -1, &stmt3, 0); int read_book=0;
                        while(sqlite3_step(stmt3)!=SQLITE_DONE && nr_carti<10)
                        {
                              cname=sqlite3_column_text(stmt3, 0);
                              caut=sqlite3_column_text(stmt3,1);
                              add_isbn=sqlite3_column_int(stmt3, 2);
                              rating=sqlite3_column_double(stmt3, 3);
                              // verific daca a citit-o 
                              sprintf(sel4, "select 2 from cautdesc where username='%s' and numecarte='%s' and descarcare=1", aux1, cname);
                              sqlite3_prepare_v2(db, sel4, -1, &stmt4, 0); int read_book=0;
                              while(sqlite3_step(stmt4)!=SQLITE_DONE)
                              {
                                    read_book++;
                              }
                              if(read_book==0)
                              {
                                    if(la_fel[add_isbn]==0)
                                    {
                                          nr_carti++;
                                          if(write(client, cname, 300)<0)
                                          {
                                                perror("eroare la trimitere carte recomandata");
                                          }
                                          if(write(client, caut, 300)<0)
                                          {
                                                perror("eroare la trimitere carte recomandata");
                                          }   
                                          printf("%s, %f \n", cname, rating);
                                    }      
                              }                  
                        }
                        
      }
      if(nr_in==0)
      {
            const unsigned char* nume_carterec;
            const unsigned char* autor;
            char sel1[3000]; int nr_de_carti_recomandate=0; int utiz; float ratin;
            sqlite3_stmt* stmt2;
            sprintf(sel1, "select carte, autor, rating, utilizrating, ISBN from librarie l where not exists (select 1 from cautdesc where username='%s' and UPPER(gen)=UPPER(l.gen) and descarcare=1) order by rating desc", aux1);
            sqlite3_prepare_v2(db, sel1, -1, &stmt2, 0);
            while(sqlite3_step(stmt2)!=SQLITE_DONE && nr_de_carti_recomandate<10)
            {
                  nr_de_carti_recomandate++;
                  nume_carterec=sqlite3_column_text(stmt2,0);
                  autor=sqlite3_column_text(stmt2, 1);
                  ratin=sqlite3_column_double(stmt2, 2);
                  utiz=sqlite3_column_int(stmt2, 3);
                  add_isbn=sqlite3_column_int(stmt2, 4);
                  if(la_fel[add_isbn]==0)
                   {     
                         la_fel[add_isbn]++;
                         if(write(client, nume_carterec, 300)<0)
                        {
                              perror("eroare la trimitere carte recomandata");
                        }
                        if(write(client, autor, 300)<0)
                        {
                              perror("eroare la trimitere carte recomandata");
                        }
                   }
                  printf("%s %f, %d\n",nume_carterec,  ratin, utiz);
            }
      }                    

}
void handler(int sig)
{
  pid_t pid;
  pid=wait(NULL);
}
void crearelib(sqlite3* db)
{   
    char* err;
      int rc=sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS client(username VARCHAR2(300) PRIMARY KEY, parola VARCHAR2(300), cautari INT)", NULL, NULL, &err);
    if(rc!= SQLITE_OK)
    {
        printf("%s error",err);
    }
    rc=sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS cautdesc(username VARCHAR2(300), numecarte VARCHAR2(300), autor VARCHAR2(300), gen VARCHAR2(300), subgen VARCHAR2(100), cautare INT, descarcare INT)", NULL, NULL, &err);
    if(rc!= SQLITE_OK)
    {
        printf("%s error",err);
    }
    rc=sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS librarie(carte VARCHAR2(300), autor VARCHAR2(300), an_aparitie INT, gen VARCHAR2(300), subgen VARCHAR2(300), rating REAL, ISBN int PRIMARY KEY, utilizrating int)", NULL, NULL, &err);
    if(rc!= SQLITE_OK)
    {
        printf("%s error",err);
    }
    rc=sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS genautor(nume VARCHAR2(300), gen VARCHAR2(300))", NULL, NULL, &err);
    if(rc!= SQLITE_OK)
    {
        printf("%s error",err);
    }
    char query[300];
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Ciuleandra', 'Liviu Rebreanu', 1927, 'beletristica','psihologic', 0 , 5013, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("Baza de date este la zi\n");  
                 }
    else{            
    bzero(query,300);
    strcpy(query,"insert into genautor VALUES ('Mihai Eminescu', 'liric')");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor1");  
          perror("eroare: ");                         }

    bzero(query,300);
    strcpy(query,"insert into genautor VALUES ('Mihai Eminescu', 'poem')");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor2");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into genautor VALUES ('Liviu Rebreanu', 'realism')");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor3");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into genautor VALUES ('Mircea Eliade', 'beletristica')");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor4");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into genautor VALUES ('Mircea Eliade', 'fantastic')");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor5");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into genautor VALUES ('Mircea Eliade', 'filosofie')");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare auto6r");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into genautor VALUES ('Agatha Christie', 'politist')");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor7");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Ion', 'Liviu Rebreanu', 1920, 'realism','obiectiv', 0 , 5000, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Enigma Otiliei', 'George Calinescu', 1938, 'realism','obiectiv', 0 , 5001, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Zece negrii mititei', 'Agatha Christie', 1939, 'politist','obiectiv', 0 , 5002, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Crima din Orient Express', 'Agatha Christie', 1934, 'politist','obiectiv', 0 , 5003, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Cine l-a ucis pe Roger Ackroyd?', 'Agatha Christie', 1926, 'politist','subiectiv', 0 , 5004, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Luceafarul', 'Mihai Eminescu', 1883, 'poem','obiectiv', 0 , 5005, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Floare albastra', 'Mihai Eminescu', 1873, 'liric','subiectiv', 0 , 5006, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Somnoroase pasarele', 'Mihai Eminescu', 1868, 'liric','subiectiv', 0 , 5007, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Hotul de carti', 'Markus Zusak', 2005, 'beletristica','obiectiv', 0 , 5008, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Pacienta tacuta', 'Alex Michaelides', 20, 'beletristica','obiectiv', 0 , 5009, 0 )");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Romanul adolescentului miop', 'Mircea Eliade', 1920, 'beletristica','obiectiv', 0 , 5010, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('La tiganci', 'Mircea Eliade', 1930, 'fantastic','subiectiv', 0 , 5011, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
        bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Sacrul si profanul', 'Mircea Eliade', 1957, 'filosofie','obiectiv', 0 , 5012, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    bzero(query,300);
    strcpy(query,"insert into librarie VALUES ('Ciuleandra', 'Liviu Rebreanu', 1927, 'beletristica','psihologic', 0 , 5013, 0)");
    rc=sqlite3_exec(db, query,NULL, NULL, &err);
    if(rc!=SQLITE_OK)
    {
          printf("eroare la adaugare autor");  
          perror("eroare: ");                         }
    }
}

