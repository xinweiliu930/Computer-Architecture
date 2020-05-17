#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */
      
#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255

typedef struct {
       int address;
       char label[MAX_LABEL_LEN+1];
      } TableEntry;
     TableEntry symbolTable[MAX_SYMBOLS];
    /*Define a Table*/

int isOpcode(char*Opcode)
{
   if (strcmp(Opcode,"add")==0)
     return (0);
   else if (strcmp(Opcode,"and")==0)
     return (0);
   else if (strcmp(Opcode,"brn")==0)
     return (0);
   else if (strcmp(Opcode,"brz")==0)
     return (0);
   else if (strcmp(Opcode,"brp")==0)
     return (0);
  else  if (strcmp(Opcode,"brnz")==0)
     return (0);
   else if (strcmp(Opcode,"brnp")==0)
     return (0);
   else if (strcmp(Opcode,"brzp")==0)
     return (0);
   else if (strcmp(Opcode,"brnzp")==0)
     return (0);
   else if (strcmp(Opcode,"br")==0)
     return (0);
   else if (strcmp(Opcode,"halt")==0)
     return (0);
   else if (strcmp(Opcode,"jmp")==0)
     return (0);
   else if (strcmp(Opcode,"jsr")==0)
     return (0);
   else if (strcmp(Opcode,"jsrr")==0)
     return (0);
   else if (strcmp(Opcode,"ldb")==0)
     return (0);
   else if (strcmp(Opcode,"ldw")==0)
     return (0);
  else  if (strcmp(Opcode,"lea")==0)
     return (0);
   else if (strcmp(Opcode,"nop")==0)
     return (0);
  else  if (strcmp(Opcode,"not")==0)
     return (0);
   else if (strcmp(Opcode,"ret")==0)
     return (0);
   else if (strcmp(Opcode,"lshf")==0)
     return (0);
   else if (strcmp(Opcode,"rshfl")==0)
     return (0);
   else if (strcmp(Opcode,"rshfa")==0)
     return (0);
  else  if (strcmp(Opcode,"rti")==0)
     return (0);
  else  if (strcmp(Opcode,"stb")==0)
     return (0);
   else if (strcmp(Opcode,"stw")==0)
     return (0);
  else  if (strcmp(Opcode,"trap")==0)
     return (0);
  else  if (strcmp(Opcode,"xor")==0)
     return (0);
   else return (-1);
}


int trans(char*Arg)
{
if (strcmp(Arg,"r0")==0)
     return 0;
else if (strcmp(Arg,"r1")==0)
     return 1;
else if (strcmp(Arg,"r2")==0)
     return 2;
else if (strcmp(Arg,"r3")==0)
     return 3;
else if (strcmp(Arg,"r4")==0)
     return 4;
else if (strcmp(Arg,"r5")==0)
     return 5;
else if (strcmp(Arg,"r6")==0)
     return 6;
else if (strcmp(Arg,"r7")==0)
     return 7;
else exit(4);
}

enum
	{
	   DONE, OK, EMPTY_LINE
	};
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
	)
	{
           
	   char * lRet, * lPtr;
	   int i;
	   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
		return( DONE );
	   for( i = 0; i < strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );
	   
           /* convert entire line to lowercase */
	   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	   /* ignore the comments */
	   lPtr = pLine;

	   while( *lPtr != ';' && *lPtr != '\0' &&
	   *lPtr != '\n' ) 
		lPtr++;

	   *lPtr = '\0';
	   if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
		return( EMPTY_LINE );

	   if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
	   {
		*pLabel = lPtr; 
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   }
	   
           *pOpcode = lPtr; //printf("%s\n",*pOpcode);

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   
           *pArg1 = lPtr; //printf("%s\n",*pArg1);
	   
           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg2 = lPtr;
	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg3 = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg4 = lPtr;

	   return( OK );
	}

	/* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */

int toNum( char * pStr )
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
     pStr++;
     if( *pStr == '-' )				/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	 printf("Error: invalid decimal operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}

int adr(char*label)
{   int q=0;
  for (int j=0;j< MAX_SYMBOLS;j++)
      {
            if (strcmp(label,symbolTable[j].label)==0)
              {q=1;
              return symbolTable[j].address;}

       }
   if(q==0)
   exit(1);
}
            

int adr(char*label);
int trans(char*Arg);
int toNum( char * pStr );
int isOpcode(char*Opcode);
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
	);

int main(int argc, char* argv[]) {
     FILE* infile = NULL;
     FILE* outfile = NULL;	
     char *prgName   = NULL;
     char *iFileName = NULL;
     char *oFileName = NULL;
     char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
	        *lArg2, *lArg3, *lArg4;

     int lRet,pc=0,pci=0,beg=0,i=0,reg=0,tem=0,offset=0,tem1=0;

     prgName   = argv[0];
     iFileName = argv[1];
     oFileName = argv[2];



/* open the source file */
     infile = fopen(argv[1], "r");
     outfile = fopen(argv[2], "w");
		 
     if (!infile) {
       printf("Error: Cannot open file %s\n", argv[1]);
       exit(4);
		 }
     if (!outfile) {
       printf("Error: Cannot open file %s\n", argv[2]);
       exit(4);
     }

     /* Do stuff with files */
    


    do
	   {    
		lRet = readAndParse( infile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
		if( lRet != DONE && lRet != EMPTY_LINE )
		{          
			    pci=pci+2;
                            if (strcmp(lOpcode, ".orig")==0)
                            {  if(toNum(lArg1)%2==0&&toNum(lArg1)>=0&&toNum(lArg1)<65535) 
                               {pc=toNum(lArg1);
                               pci=pc-2;
                               beg=1;}
                               else exit(3);
                                                      }
                          
                            if (strcmp(lOpcode, ".end")==0)
                            {  
                               beg=0;
                            }
                            
                            if (strlen(lLabel)!=0&&beg==1)
                            {                              
                               for(int k=0;k<i;k++)
                              {if(strcmp(lLabel,symbolTable[k].label)==0)
                               exit(4);
                              }
                               for(int l=0;l<strlen(lLabel);l++)
                              { //printf("%d\n",isalnum(lLabel[l]));
                                if(isalnum(lLabel[l])==0)
                                exit(4);}
                                
                               //printf("%d\n",isalnum(*lLabel));
                               if(*lLabel!='x'&&strcmp(lLabel,"in")!=0&&strcmp(lLabel,"out")!=0&&strcmp(lLabel,"getc")!=0&&strcmp(lLabel,"puts")!=0&&strlen(lLabel)<=20&&isOpcode(lLabel)==-1&&strcmp(lLabel,".orig")!=0&&strcmp(lLabel,".end")!=0&&strcmp(lLabel,".fill")!=0&&isalnum(*lLabel)!=0&&i<255)
                              {strcpy (symbolTable[i].label,lLabel);//
                              symbolTable[i].address = pci; printf("%x\t%s\n",symbolTable[i].address,symbolTable[i].label);
                              i=i++;}
                              else exit(4);
 
                            }
                              
		}
	   } while( lRet != DONE );
                                     /*firstpass*/

rewind(infile);

     do
{               //printf("%s\n",lOpcode);
		lRet = readAndParse( infile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
		if( lRet != DONE && lRet != EMPTY_LINE )
		{           if(beg==1&&isOpcode(lOpcode)==-1&&strcmp(lOpcode,".orig")!=0&&strcmp(lOpcode,".end")&&strcmp(lOpcode,".fill"))
                            { exit(2);}
                            //printf("%s\n",lOpcode);
		            pci=pci+2;
                            if (beg==0&&strcmp(lOpcode, ".orig")==0)
                            {
                               pc=toNum(lArg1);
                               pci=pc-2;
                               beg=1;
                               fprintf(outfile,"0x%.4X\n",pc);                          }
                                         /*orig*/
                            if (strcmp(lOpcode, ".end")==0)
                            {
                               beg=0;
                            }
                                        /*end*/
                            if (strcmp(lOpcode,".fill")==0)
                            { reg= toNum(lArg1); 
                              fprintf(outfile,"0x%.4X\n",reg);
                            }
                                         /*fill*/
                            if (beg==1&&strcmp(lOpcode, "add")==0)
                            { if(*lArg1=='r'&&*lArg2=='r'&&(*lArg3=='#'||*lArg3=='x'||*lArg3=='r'))
                             {tem=1;tem1=0x1F;
                               if (*lArg3=='r')
                               {reg=tem*4096+trans(lArg1)*512+trans(lArg2)*64+trans(lArg3);}
                               else if (*lArg3=='#'||*lArg3=='x')
                                {if(toNum(lArg3)>=-16&&toNum(lArg3)<0)
                                 {tem1=tem1&toNum(lArg3);
                                  reg=tem*4096+trans(lArg1)*512+trans(lArg2)*64+32+tem1;}
                                 else if(toNum(lArg3)<16&&toNum(lArg3)>=0)  
                                 {reg=tem*4096+trans(lArg1)*512+trans(lArg2)*64+32+toNum(lArg3);}
                                 else exit(3);
                                 }
                               fprintf(outfile,"0x%.4X\n",reg);
                             } else exit(4);
                            } 
	                                 /*add*/

                            if (beg==1&&strcmp(lOpcode, "and")==0)
                            { if(*lArg1=='r'&&*lArg2=='r'&&strlen(lArg3)!=0) 
                              { tem=5;tem1 = 0x1F;
                               if (*lArg3=='r')
                               {reg=tem*4096+trans(lArg1)*512+trans(lArg2)*64+trans(lArg3);}
                               else if (*lArg3=='#'||*lArg3=='x') 
                               {if(toNum(lArg3)>=-16&&toNum(lArg3)<0)
                                 {tem1=tem1&toNum(lArg3);
                                  reg=tem*4096+trans(lArg1)*512+trans(lArg2)*64+32+tem1;}
                                 else if(toNum(lArg3)<16&&toNum(lArg3)>=0)  
                                 {reg=tem*4096+trans(lArg1)*512+trans(lArg2)*64+32+toNum(lArg3);}
                                  else exit(3);
                                }
                                fprintf(outfile,"0x%.4X\n",reg);
                              } else exit(4);
                            } 
                                        /*and*/

                            if (beg==1&&strcmp(lOpcode,"brn")==0)
                            { if(strlen(lArg1)!=0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                              { tem = 0x800;
                               offset = (adr(lArg1)-pci-2)/2;
                               if (offset>=-256&&offset<256)
                               {tem1 = 0x1FF;
                               reg = (offset&tem1)|tem;
                               fprintf(outfile,"0x%.4X\n",reg);}
                               else exit(4);
                               }else exit(4);
                             } 
                                           /*brn*/


                            if (beg==1&&strcmp(lOpcode,"brz")==0)
                            {if(strlen(lArg1)!=0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                             { tem = 0x400;
                               offset = (adr(lArg1)-pci-2)/2;
                               if (offset>=-256&&offset<256)
                               {tem1 = 0x1FF;
                               reg = (offset&tem1)|tem;}
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg);
                              }else exit(4);
                             } 
                                           /*brz*/

                            if (beg==1&&strcmp(lOpcode,"brp")==0)
                            {if(strlen(lArg1)!=0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                              { tem = 0x200;
                               offset = (adr(lArg1)-pci-2)/2;
                               if (offset>=-256&&offset<256)
                               {tem1 = 0x1FF;
                               reg = (offset&tem1)|tem;}
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg);
                              }else exit(4);

                             } 
                                           /*brp*/

                            if (beg==1&&strcmp(lOpcode,"brnz")==0)
                            {if(strlen(lArg1)!=0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                              {tem = 0xC00;
                               offset = (adr(lArg1)-pci-2)/2;
                               if (offset>=-256&&offset<256)
                               {tem1 = 0x1FF;
                               reg = (offset&tem1)|tem;}
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg);
                              }else exit(4);
                             } 
                                           /*brnz*/

                            if (beg==1&&strcmp(lOpcode,"brnp")==0)
                            {if(strlen(lArg1)!=0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                              { tem = 0xA00;
                               offset = (adr(lArg1)-pci-2)/2;
                               if (offset>=-256&&offset<256)
                               {tem1 = 0x1FF;
                               reg = (offset&tem1)|tem;}
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg);
                              }else exit(4);
                             } 
                                           /*brnp*/

                            if (beg==1&&strcmp(lOpcode,"br")==0)
                            {if(strlen(lArg1)!=0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                              { tem = 0xE00;
                               offset = (adr(lArg1)-pci-2)/2;
                               if (offset>=-256&&offset<256)
                               {tem1 = 0x1FF;
                               reg = (offset&tem1)|tem;}
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg);
                              }else exit(4);
                             } 
                                           /*brnzp*/

                            if (beg==1&&strcmp(lOpcode,"brzp")==0)
                            {if(strlen(lArg1)!=0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                              { tem = 0x600;
                               offset = (adr(lArg1)-pci-2)/2;
                               if (offset>=-256&&offset<256)
                               {tem1 = 0x1FF;
                               reg = (offset&tem1)|tem;}
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg);
                              }else exit(4);
                             } 
                                           /*brzp*/

                            if (beg==1&&strcmp(lOpcode,"brnzp")==0)
                            {if(strlen(lArg1)!=0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                              { tem = 0xE00;
                               offset = (adr(lArg1)-pci-2)/2;
                               if (offset>=-256&&offset<256)
                               {tem1 = 0x1FF;
                               reg = (offset&tem1)|tem;}
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg);
                              }else exit(4);
                             } 
                                           /*br*/



                           if (beg==1&&strcmp(lOpcode,"halt")==0)
                           {if(strlen(lArg1)==0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                              {
                               reg = 0xF025;
                               fprintf(outfile,"0x%.4X\n",reg);
                              }else exit(4);
                           }
                                            /*halt*/

                           if (beg==1&&strcmp(lOpcode,"jmp")==0)
                           {if(*lArg1=='r'&&strlen(lArg2)==0&&strlen(lArg3)==0)
                              { tem = 12;
                               reg = tem*4096+trans(lArg1)*64;
                               fprintf(outfile,"0x%.4X\n",reg); 
                              }else exit(4);
                           }
                                          /*jmp*/

                           if (beg==1&&strcmp(lOpcode,"jsr")==0)
                           {if(strlen(lArg1)!=0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                             {tem = 0x7FF;
                               tem1 = 0x4800;
                               offset = (adr(lArg1)-pci-2)/2;
                               if (offset>=-1024&&offset<1024)
                               {offset = offset&tem;
                                reg = offset|tem1;
                               }
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg);
                              }else exit(4); 
                           }
                                          /*jsr*/

                           if (beg==1&&strcmp(lOpcode,"jsrr")==0)
                           {if(*lArg1=='r'&&strlen(lArg2)==0&&strlen(lArg3)==0)
                              { tem = 4;
                               reg = tem*4096+trans(lArg1)*64;
                               fprintf(outfile,"0x%.4X\n",reg); 
                              }else exit(4);
                           }
                                          /*jsrr*/

                           if (beg==1&&strcmp(lOpcode,"ldb")==0)
                           {if(*lArg1=='r'&&*lArg2=='r'&&(*lArg3=='#'||*lArg3=='x'))
                              {tem = 2;
                               tem1 = 0x001F;
                               offset = toNum(lArg3);
                               if(offset>=-16&&offset<0)
                               {offset = offset&tem1;
                                reg = tem*4096+trans(lArg1)*512+trans(lArg2)*64+offset;
                               }
                              else if(offset>=0&&offset<16)
                               {reg = tem*4096+trans(lArg1)*512+trans(lArg2)*64+offset;
                               } 
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg); 
                              }else exit(4);
                           }
                                          /*ldb*/

                           if (beg==1&&strcmp(lOpcode,"ldw")==0)
                           {if(*lArg1=='r'&&*lArg2=='r'&&(*lArg3=='#'||*lArg3=='x'))
                             {  tem = 6;
                               tem1 = 0x001F;
                               offset = toNum(lArg3);
                               if(offset>=-16&&offset<0)
                               {offset = offset&tem1;
                                reg = tem*4096+trans(lArg1)*512+trans(lArg2)*64+offset;
                               }
                              else if(offset>=0&&offset<16)
                               {reg = tem*4096+trans(lArg1)*512+trans(lArg2)*64+offset;
                               } 
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg); 
                             }else exit(4);
                           }
                                          /*ldw*/

                           if (beg==1&&strcmp(lOpcode,"lea")==0)
                           {if(*lArg1=='r'&&strlen(lArg2)!=0&&strlen(lArg3)==0)   
                             {  tem = 14;
                               tem1 = 0x1FF;
                               offset = (adr(lArg2)-pci-2)/2;
                              if(offset>=-256&&offset<256)
                              { offset = offset&tem1;
                               reg = tem*4096+trans(lArg1)*512+offset;
                              }
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg); 
                            }else exit(4);
                           }
                                          /*lea*/

                          

                           if (beg==1&&strcmp(lOpcode,"not")==0)
                           {if(*lArg1=='r'&&*lArg2=='r'&&strlen(lArg3)==0) 
                              { tem = 9;
                               reg = tem*4096+trans(lArg1)*512+trans(lArg2)*64+63;
                               fprintf(outfile,"0x%.4X\n",reg); 
                              }else exit(4);
                           }
                                          /*not*/

                           if (beg==1&&strcmp(lOpcode,"nop")==0)
                           {
                              
                               reg = 0x000;
                               fprintf(outfile,"0x%.4X\n",reg); 
                           }
                                          /*nop*/

                           if (beg==1&&strcmp(lOpcode,"ret")==0)
                           {if(strlen(lArg1)==0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                               
                              {reg = 0xC1C0;
                               fprintf(outfile,"0x%.4X\n",reg); 
                              }else exit(4);
                           }
                                          /*ret*/

                           if (beg==1&&strcmp(lOpcode,"rti")==0)
                           {if(strlen(lArg1)==0&&strlen(lArg2)==0&&strlen(lArg3)==0)
                              { 
                               reg = 0x8000;
                               fprintf(outfile,"0x%.4X\n",reg); 
                              }else exit(4);
                           }
                                          /*rti*/

                           if (beg==1&&strcmp(lOpcode,"lshf")==0)
                           { if(*lArg1=='r'&&*lArg2=='r'&&strlen(lArg3)!=0)
                             {  tem = 13;
                               if(toNum(lArg3)>=0&&toNum(lArg3)<15)
                               {reg = tem*4096+trans(lArg1)*512+trans(lArg2)*64+toNum(lArg3);
                               }
                               else exit(3);
                               fprintf(outfile,"0x%.4X\n",reg); 
                             }else exit(4);
                           }
                                          /*lshf*/

                           if (beg==1&&strcmp(lOpcode,"rshfl")==0)
                           { if(*lArg1=='r'&&*lArg2=='r'&&strlen(lArg3)!=0)
                               
                               { tem = 13;
                               if(toNum(lArg3)>=0&&toNum(lArg3)<15)
                               {reg = tem*4096+trans(lArg1)*512+trans(lArg2)*64+16+toNum(lArg3);
                               }
                               else exit(3);
                               fprintf(outfile,"0x%.4X\n",reg); 
                             }else exit(4);
                           }
                                          /*rshfl*/

                           if (beg==1&&strcmp(lOpcode,"rshfa")==0)
                           { if(*lArg1=='r'&&*lArg2=='r'&&strlen(lArg3)!=0)
                               
                              { tem = 13;
                               if(toNum(lArg3)>=0&&toNum(lArg3)<15)
                               {reg = tem*4096+trans(lArg1)*512+trans(lArg2)*64+48+toNum(lArg3);
                               }
                               else exit(3);
                               fprintf(outfile,"0x%.4X\n",reg); 
                              }else exit(4);
                           }
                                          /*rshfa*/

                           if (beg==1&&strcmp(lOpcode,"stb")==0)
                           { if(*lArg1=='r'&&*lArg2=='r'&&strlen(lArg3)!=0)
                               
                              { tem = 3;
                               tem1 = 0x3F;
                               offset = tem1&toNum(lArg3);
                               if(toNum(lArg3)>=-32&&toNum(lArg3)<32)
                               {reg = tem*4096+trans(lArg1)*512+trans(lArg2)*64+offset;
                               }
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg);
                             }else exit(4); 
                           }
                                          /*stb*/

                           if (beg==1&&strcmp(lOpcode,"stw")==0)
                           { if(*lArg1=='r'&&*lArg2=='r'&&strlen(lArg3)!=0)
                               
                             {  tem = 7;
                               tem1 = 0x3F;
                               offset = tem1&toNum(lArg3);
                               if(toNum(lArg3)>=-32&&toNum(lArg3)<32)
                               {reg = tem*4096+trans(lArg1)*512+trans(lArg2)*64+offset;
                               }
                               else exit(4);
                               fprintf(outfile,"0x%.4X\n",reg); 
                             }else exit(4);
                           }
                                          /*stw*/

                           if (beg==1&&strcmp(lOpcode,"trap")==0)
                           {
                               
                               
                               if(strcmp(lArg1,"x25")==0)
                               reg = 0xF025;
                               fprintf(outfile,"0x%.4X\n",reg); 
                           }
                                          /*trap*/

                           if (beg==1&&strcmp(lOpcode,"xor")==0)
                           { if(*lArg1=='r'&&*lArg2=='r'&&*lArg3=='r')
                               
                             {  tem = 9;tem1=0x1F;
                               if (*lArg3=='r')
                               {reg=tem*4096+trans(lArg1)*512+trans(lArg2)*64+trans(lArg3);}
                               else if (*lArg3=='#'||*lArg3=='x')
                                {if(toNum(lArg3)>=-16&&toNum(lArg3)<0)
                                 {tem1=tem1&toNum(lArg3);
                                  reg=tem*4096+trans(lArg1)*512+trans(lArg2)*64+32+tem1;}
                                 else if(toNum(lArg3)<16&&toNum(lArg3)>=0)  
                                 {reg=tem*4096+trans(lArg1)*512+trans(lArg2)*64+32+toNum(lArg3);}
                               else exit(3);                                
                                 }
                               fprintf(outfile,"0x%.4X\n",reg);
                              }else exit(4); 
                           }
                                          /*xor*/
                          





		}
	   } while( lRet != DONE );
	                           /*second pass*/

   
       exit(0); 
     fclose(infile);
     fclose(outfile);
}


