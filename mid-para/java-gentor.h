
/*

+	JAVA Code Generator based on LGNOSIA , part of SC mid-para

+	'Semo Compiler' is a multi-objective compiler which is developing under the terms of the 
+	GNU general public license as published by the Free Software Foundation.
+	The project lunched by Jelo Wang since 2.Feb.2008 from 'Techniques of Knowledge' community. 

+	You can redistribute it and/or modify it under the terms of the gnu general public version 3 of 
+	the license as published by the free software foundation.this program is distributed in the hope 
+	that it will be useful,but without any warranty.without even the implied warranty of merchantability 
+	or fitness for a particular purpose.																					
																												
+	(C)	ͻ�ǿ�ԴTechniques of Knowledge
+		an open source community since 2008
+		Community : http://www.tok.cc
+		Contact Us : jelo.wang@gmail.com

+		-Thanks to Our Committers and Friends
+		-Best Wish to all who Contributed and Inspired
+		-Techniques of Knowledge 
+		-�»����������ףԸ
+		-ͻ�ǿ�Դ����

*/ 

# ifndef __JAVA_GENTOR
# define __JAVA_GENTOR
 
# include "symbole.h"
# include "lgnosia.h"
# include "sccl.h"

typedef struct {

	//	author : Jelo Wang
	//	since : 20100107
	//	(C)TOK
	
	int form ;
	int stop ;
	int delt ;
	int label ;
	int lgnosia ;

	//	a set of lgnosia
	SCClList* tree ;

	SCClStack identor ;
	SCClStack scopestack ;
	SCClStack lgastack ;	
	
} JAVA_GENTOR ;

# endif

char* gentor_java_run ( char* file ) ;

