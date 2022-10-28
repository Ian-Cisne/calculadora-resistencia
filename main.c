#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*Programa que te calcula al resistencia total de un circuito simple
Funciona de la siguiente manera:
1 - Indico cuántas resistencias que tengo.
2 - Ingreso el valor de las resistencias.
3 - Ingreso la operación como una cadena de caracteres.
La operación no se valida, se da por hecho que está bien escrita
Podría tener, por ejemplo, las siguientes formas para 3 resistencias:
			R2p(R3sR1)
			R1s(R2pR3)
			(R2pR1)pR2
O para 4 resistencias:
			(R2pR1)s(R1pR3)
			(R2sR3)s(R1pR4)
*/

typedef enum t_type_node{
	VOID,
	OPER,
	HOJA
}t_type_node;

typedef struct t_nodo{
	t_type_node type_node;	
	float value;
	t_nodo* parent;
	t_nodo* lf_child;
	t_nodo* rg_child;
}t_nodo;

t_nodo* calculate(t_nodo*);

float paralelo(float, float);
float serie(float, float);

int main(){
	unsigned short cant_resistencias;
	printf("Ingrese la cantidad de resistencias: ");
	scanf("%d", &cant_resistencias);
	
	float *resistencias = (float*)malloc(sizeof(float) * (cant_resistencias + 1));
	
	printf("Ingrese las %d resistencias:", cant_resistencias); 
	
	for(int i = 1; i <= cant_resistencias; i++)
		scanf("%f", &resistencias[i]);
		
	char operacion[10*cant_resistencias];
	printf("Ingrese la operacion (por ejemplo, \"(R2pR3)sR1\"):");
	fflush(stdin);
	scanf("%s", operacion);
	int cant_nodos = 2 * cant_resistencias;
	t_nodo *arbol = (t_nodo*)malloc(sizeof(t_nodo) * cant_nodos);

	for(int i = 1; i <= cant_nodos; i++){
		arbol[i].type_node = VOID;
		arbol[i].value = -1;
		arbol[i].lf_child = NULL;
		arbol[i].rg_child = NULL;
		arbol[i].parent = NULL;
	}
	
	int atom_size,
		position_s = 0,
		position_t = 0;
	
	t_nodo *actual_pos_t = &arbol[0];
	
	char last_char = '\0';
	
	while(operacion[position_s] != '\0'){

		switch(operacion[position_s]){
		case '(':{
				position_t++;
				if(last_char == 's'
					|| last_char == 'p'){		
					actual_pos_t->rg_child = &arbol[position_t];
					actual_pos_t->rg_child->parent = actual_pos_t;
					actual_pos_t = &arbol[position_t];
				}else{
					actual_pos_t->lf_child = &arbol[position_t];
					actual_pos_t->lf_child->parent = actual_pos_t;
					actual_pos_t = &arbol[position_t];
				}
				atom_size = 1;
				break;
			}
		case ')':{
				t_nodo *tha_parent = actual_pos_t->parent;
				actual_pos_t = tha_parent;
				atom_size = 1;
				break;
			}
		case 'R':{
				position_t++;
				int pivot_pos = position_s+1;
				char res_num[4] = "";
				atom_size = 1;
				
				while(isdigit(operacion[pivot_pos])){
					strncat(res_num, &operacion[pivot_pos], 1);
					atom_size+=1;
					pivot_pos+=1;
				}
				
				unsigned char num_res = atoi(res_num);
				t_nodo *tha_child;
				
				if(last_char == 's'
					|| last_char == 'p'){
					actual_pos_t->rg_child = &arbol[position_t];
					tha_child = actual_pos_t->rg_child;
				}else{
					actual_pos_t->lf_child = &arbol[position_t];
					tha_child = actual_pos_t->lf_child;
				}
				
				tha_child->type_node = HOJA;
				tha_child->value = resistencias[num_res];
				break;
			}
		case 'p':{
				actual_pos_t->type_node = OPER;
				actual_pos_t->value = 2;
				atom_size = 1;
				break;
			}
		case 's':{
				actual_pos_t->type_node = OPER;
				actual_pos_t->value = 1;
				atom_size = 1;
				break;
			}
		}
		last_char = operacion[position_s];
		position_s += atom_size;
	}
	
	calculate(arbol);
	
	printf("Resultado: %.2f", arbol->value );
	
	return 0;
}

t_nodo* calculate(t_nodo* actual){
	if(actual->lf_child->type_node == OPER)
		calculate(actual->lf_child);
	if(actual->rg_child->type_node == OPER)
		calculate(actual->rg_child);
	
	if(actual->value == 1)
		actual->value = serie(actual->lf_child->value, actual->rg_child->value);
	else if(actual->value == 2)
		actual->value = paralelo(actual->lf_child->value, actual->rg_child->value);
		
	actual->type_node = HOJA;
	
}

float paralelo(float a, float b){
	return 1/(1/a + 1/b);
}

float serie(float a, float b){
	return a + b;
}

