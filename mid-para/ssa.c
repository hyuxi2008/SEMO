
/*

+	SSA Form Generator, part of SC mid-para

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

# include "sccl.h"
# include "schal.h"
# include "symbole.h"

//	what a junk codes it is needs 256 ssa alias !
# define SSA_MULTI_ALIAS_MAX 256


///
//	ʶ����ر����ܼ򵥣�����ֻҪ����һ������������г��ֵ�symbol����������
//	ע���ǰ�symbolÿ�εı�������������Ȼ����������������ĳsymbol��һ����ֵ������
//	���ң��������Ƿ��ж��ر������ã�����������ֻ��һ�����ڵ����ԣ���Ч�Ķ�������
//	һ�������һ��������ı�����

//	��α�����Ч����?

//	���ر������ֵ������Ƿ�����������Ƕ�ף�����������ʣ�һ�����ŵĶ��ر���������������
//	��һ��Ϊ������Ƕ��ǰ�ı������ڶ���ΪǶ����������ڵı�����
//	�ڴ����ǰѵ�һ����������Ϊancestor ������ζ��������Ҫ��ͬһ���������ڵ�
//	����������������ȡ���ر���ʱ���Ȼ�ȡͬһ�������ڵ�ancestor ��
//

typedef struct {

	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	

	//	an abstractive symbol
	int symbol ;
	//	a number between 0,...n
	int alias ;
	//	the last flow has apeared
	int flow ;
	int scope ;

	struct {

		int totall ;
		//	the smallest scope of any of a alias
		//	�����������ʶ���Ƿ��ж��ر���
		int scope ;
		int alias[ SSA_MULTI_ALIAS_MAX ] ;
		
	} MultiAlias ;
		
} SSA ;

typedef struct {

	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	

	int symbol ;
	int alias ;
	int scope ;
	
} ANCESTOR ;

static int SsaMultiAliasEnable = 0 ;
static SCClList aliaslist = { 0 } ;
static SCClList ancestorlist = { 0 } ;

static int SsaGetMultiAliasValid ( int symbol , int flow , int scope ) {

	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	

	//	�ж��Ƿ�ɻ�ö��ر���
	//	ֻҪ������flowǨ�ƾ���Ҫ�ж�

	SSA* ssa = 0 ;
	SCClList* looper = 0 ;
	
	for ( looper = aliaslist.head ; looper ; looper = looper->next ) {
		ssa = (SSA* ) looper->element ;
		if ( (int ) symbol == (int ) ssa->symbol ) {
			break ;
		}
	}	
	
	if ( 0 != looper ) {		
		//	���ر��������������ȴ��ڵ�ǰ���
		//	�����Ѿ�������flowǨ��
		if ( ssa->flow != flow ) {
			return 1 ;
		}
		return 0 ;
	}

	return 0 ;
	
}

static void SsaSaveAncestorAlias ( int symbol , int alias , int scope ) {

	ANCESTOR* ancestor = 0 ;

	SCClListSetIterator ( &ancestorlist , SCCLLISTSEEK_HEAD ) ;
	
	for ( ; SCClListIteratorPermit ( &ancestorlist ) ; SCClListListIteratorNext ( &ancestorlist ) ) {

		ancestor = (ANCESTOR* ) SCClListIteratorGetElement ( &ancestorlist ) ;

		if ( 0 != ancestor && (int ) ancestor->symbol == (int ) symbol ) {
			if ( ancestor->scope == scope ) {
				//	update the lastest alias of 'symbol'
				ancestor->alias = alias ;
			} else {
				//	insert new
				ancestor = (ANCESTOR* ) SCMalloc ( sizeof(ANCESTOR) ) ;
				ASSERT(ancestor) ;
				ancestor->symbol = symbol ;
				ancestor->alias = alias ;
				ancestor->scope = scope ;		
				SCClListInsert ( &ancestorlist , (void* ) ancestor ) ;			
				
			}
		}

	}

	if ( 0 == ancestor ) {

		ancestor = (ANCESTOR* ) SCMalloc ( sizeof(ANCESTOR) ) ;
		ASSERT(ancestor) ;
		ancestor->symbol = symbol ;
		ancestor->alias = alias ;
		ancestor->scope = scope ;		
		SCClListInsert ( &ancestorlist , (void* ) ancestor ) ;
	}
		
	
}

int SsaMakeAlias ( int flow , int symbol , int scope ) {

	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	

	SSA* ssa = 0 ;
	SCClList* looper = 0 ;
	
	for ( looper = aliaslist.head ; looper ; looper = looper->next ) {
		ssa = (SSA* ) looper->element ;
		if ( (int ) symbol == (int ) ssa->symbol ) {
			break ;
		}
	}

	if ( 0 != looper ) {

		//	if there have a ssa form exist already , we just return it here as well
		//	increase it and return
		ssa->alias ++ ;
		
		//	save multi alias
		if ( SsaMultiAliasEnable ) {

			//	if it still in the same flow then dont increase totall just rewrite the last one
			if ( ssa->flow != flow ) {
				ssa->MultiAlias.totall ++ ;
			}

			//	only save the smallest one
			//	��û�ж��ر��������������õ�ǰ�������ssa->MultiAlias.scope
			//	�ȽϿ�˭����
			if ( scope > ssa->MultiAlias.scope ) {
				ssa->MultiAlias.scope = scope ;
			}

			ssa->MultiAlias.alias [ssa->MultiAlias.totall] = ssa->alias ;

		}
		ssa->flow = flow ;
		ssa->scope = scope ;
		//	���Լ������б�������ancestor������û����
		SsaSaveAncestorAlias ( symbol ,  ssa->alias , scope ) ;			
		return ssa->alias ;

	} else {

		//	else , save the symbol , and make a SSA form
		ssa = (SSA* ) SCMalloc ( sizeof(SSA) ) ;
		ASSERT(ssa) ;
		ssa->symbol = symbol ;
		ssa->alias = 0 ;
		ssa->flow = flow ;	
		ssa->scope = scope ;
		
		SCClListInsert ( &aliaslist , (void* ) ssa ) ;

		//	save multi alias
		if ( SsaMultiAliasEnable ) {
			ssa->MultiAlias.scope = scope ;			
			ssa->MultiAlias.alias [ssa->MultiAlias.totall] = ssa->alias ;
			//	���ﲻ��Ҫ�ݼ�
			//	ssa->MultiAlias.totall ++ ;			
		}

		//	���Լ������б�������ancestor������û����
		SsaSaveAncestorAlias ( symbol ,  ssa->alias , scope ) ;

		return ssa->alias ;

	}

}

int SsaGetAlias ( int symbol , int scope ) {

	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	

	SSA* ssa = 0 ;
	SCClList* looper = 0 ;
	
	for ( looper = aliaslist.head ; looper ; looper = looper->next ) {
		ssa = (SSA* ) looper->element ;
		if ( (int ) symbol == (int ) ssa->symbol ) {
			break ;
		}
	}
		
	if ( 0 != looper ) {

		//	if found , we just return it here as well
		return ssa->alias ;

	} else {
	
		return 0 ;

	}	
	
}

int SsaGetAliasFlow ( int symbol ) {

	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	

	SSA* ssa = 0 ;
	SCClList* looper = 0 ;
	
	for ( looper = aliaslist.head ; looper ; looper = looper->next ) {
		ssa = (SSA* ) looper->element ;
		if ( (int ) symbol == (int ) ssa->symbol ) {
			break ;
		}
	}
		
	if ( 0 != looper ) {

		//	if found , we just return it here as well
		return ssa->flow ;

	} else {
	
		return 0 ;

	}	
	
}

int SsaGetAncestorAlias ( int symbol , int scope ) {
	
	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	

	ANCESTOR* ancestor = 0 ;

	SCClListSetIterator ( &ancestorlist , SCCLLISTSEEK_HEAD ) ;
	
	for ( ; SCClListIteratorPermit ( &ancestorlist ) ; SCClListListIteratorNext ( &ancestorlist ) ) {

		ancestor = (ANCESTOR* ) SCClListIteratorGetElement ( &ancestorlist ) ;

		if ( 0 != ancestor && (int ) ancestor->symbol == (int ) symbol ) {
			if ( ancestor->scope == scope ) {
				return ancestor->alias ;
			}
		}

	}

	return -1 ;

	
}

void SsaMakeMultiAliasEnable () {

	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	
	
	SsaMultiAliasEnable ++ ;
	
} ;

void SsaMakeMultiAliasDisable () {

	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	
	
	SsaMultiAliasEnable -- ;

} ;

int* SsaGetMultiAlias ( int symbol , int* totall ) {

	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	
	
	SSA* ssa = 0 ;
	SCClList* looper = 0 ;
	
	for ( looper = aliaslist.head ; looper ; looper = looper->next ) {
		ssa = (SSA* ) looper->element ;
		if ( (int ) symbol == (int ) ssa->symbol ) {
			break ;
		}
	}	
	
	if ( 0 != looper ) {		
		if ( 0 != ssa->MultiAlias.totall ) {
			*totall = ssa->MultiAlias.totall + 1 ;
		}
		return ssa->MultiAlias.alias ;
	}

	return 0 ;
	
}

void SsaCleanMultiAlias ( int symbol , int flow ) {

	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	

	SSA* ssa = 0 ;
	SCClList* looper = 0 ;
	
	for ( looper = aliaslist.head ; looper ; looper = looper->next ) {
		ssa = (SSA* ) looper->element ;
		if ( (int ) symbol == (int ) ssa->symbol ) {
			break ;
		}
	}	
	
	if ( 0 != looper ) {	
		//	һ����緢����AST���ݣ�����������ر�����¼
		if ( ssa->flow != flow ) {
			ssa->MultiAlias.totall = 0 ;
		}
	}


}

char* SsaMakeMultiAliasString ( int symbol , char* name , int scope , int flow ) {

	//	author : Jelo Wang
	//	since : 20121026
	//	(C)TOK	

	//	out string "0,1,2,3,4...n"

	int looper = 0 ;
	int totallmultialias = 0 ;
	int* multialias = 0 ;

	SCClString string = { 0 } ;
	char* azonalname = 0 ;

	int ancestor = 0 ;
	int alias = 0 ;

	if ( 0 == SsaGetMultiAliasValid ( symbol , flow , scope ) ) {
		return 0 ;
	}
	
	multialias = SsaGetMultiAlias ( symbol , &totallmultialias ) ;
	
	if ( 0 == totallmultialias ) {
		return 0 ;
	}
	
	//	get ancestor first
//	ancestor = SsaGetAncestorAlias ( symbol , scope ) ;
//	if ( -1 != ancestor ) {
//		azonalname = sc_strcat ( name , SCClItoa (ancestor) ) ;
//		ASSERT(azonalname) ;
//		SCClStringAddStr ( &string , azonalname ) ;
//		SCClStringAdd( &string , ',' ) ;
//	} 
	
	for ( looper = 0 ; looper < totallmultialias ; looper ++ ) {
		
		azonalname = sc_strcat ( name , SCClItoa (multialias[looper]) ) ;
		ASSERT(azonalname) ;
		SCClStringAddStr ( &string , azonalname ) ;
		if ( (looper+1) < totallmultialias ) SCClStringAdd( &string , ',' ) ;
		SCFree ( azonalname ) ;
		
	}

	return string.data ;
	
	
}

void SsaClean () {

	//	author : Jelo Wang
	//	since : 20121027
	//	(C)TOK	

	SCClListDestroy ( &aliaslist ) ;
	sc_memset ( &aliaslist , 0 , sizeof(SCClList) ) ;
	
}


