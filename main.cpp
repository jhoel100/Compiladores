#include <cstdio>
#include <fstream>
#include <iostream>
#include <bitset>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <set>
#define NFA_MAX_ESTADOS 20
#define ALFB_MAX 20

using namespace std;

class EstadoNFA {
    public:
        int etiqueta;
        int transiciones[ALFB_MAX][NFA_MAX_ESTADOS];
        EstadoNFA()  {
            for(int i=0; i<ALFB_MAX; i++)
                for(int j=0; j<NFA_MAX_ESTADOS; j++)
                    transiciones[i][j] = -1;
        }
} *EstadosNFA;

struct EstadoDFA {
    bool Estadofinal;
    bitset<NFA_MAX_ESTADOS> NFAEstadosConsistentes;
    bitset<NFA_MAX_ESTADOS> transiciones[ALFB_MAX];
    int TransicionSimbolica[ALFB_MAX];
};

set <int> EstadosFinales_NFA;
vector <int> EstadosFinales_DFA;
vector <EstadoDFA*> EstadosDFA;
queue <int> EstadosDFAIncompletos;
int N, M; 

void ClausuraEpsilon(int state, bitset<NFA_MAX_ESTADOS> &clausura )    {
    for(int i=0; i<N && EstadosNFA[state].transiciones[0][i] != -1; i++)
        if(clausura[EstadosNFA[state].transiciones[0][i]] == 0)    {
            clausura[EstadosNFA[state].transiciones[0][i]] = 1;
            ClausuraEpsilon(EstadosNFA[state].transiciones[0][i], clausura);
        }
}

void ClausuraEpsilon(bitset<NFA_MAX_ESTADOS> state, bitset<NFA_MAX_ESTADOS> &clausura) {
    for(int i=0; i<N; i++)
        if(state[i] == 1)
            ClausuraEpsilon(i, clausura);
}

void moverNFA(int X, int A, bitset<NFA_MAX_ESTADOS> &Y)   {
    for(int i=0; i<N && EstadosNFA[X].transiciones[A][i] != -1; i++)
        Y[EstadosNFA[X].transiciones[A][i]] = 1;
}

void moverNFA(bitset<NFA_MAX_ESTADOS> X, int A, bitset<NFA_MAX_ESTADOS> &Y)   {
    for(int i=0; i<N; i++)
        if(X[i] == 1)
            moverNFA(i, A, Y);
}

int main()  {
    int i, j, X, Y, A, T, F, D;
    int t;
    vector<vector<int>> enlaces;
    vector<int> diccionario;
    string otros;
    //LENA LOS ESTADOS
    cin >>otros;
    cin >> N;
    EstadosNFA = new EstadoNFA[N];
    for(i=0; i<N; i++)    {
        cin >> EstadosNFA[i].etiqueta;
    }
    //LLENA ESTADOS DE ACEPTACION
    cin >>otros;
    cin >> F;
    for(i=0; i<F; i++)    {
        cin >> X;
        EstadosFinales_NFA.insert(X);
    }
    //LENA ENTRADAS
    cin >>otros;
    cin >> M;
    diccionario.reserve(M);
    for(i=0; i<M; i++)    {
        cin >> diccionario[i];
    }
    //LLENA TRANSICIONES
    cin >>otros;
    cin >> t;
    enlaces.reserve(t);
    for(int ii=0;ii<t;ii++){
        enlaces[ii].reserve(3);
    }
    for(int ii=0;ii<t;ii++){
        for(int jj=0;jj<3;jj++){
            cin>>enlaces[ii][jj];
        }
    }

    //CONVIERTE A FORMATO COMPACTO
    int cantidad=t;

    bool sup=false;
    for(int ii=0;ii<N;ii++){
        for(int jj=0;jj<t;jj++){
            if(ii==enlaces[jj][0] && sup){
                cantidad--;
            }
            if(ii==enlaces[jj][0] && !sup){
                sup=true;
            }
        }
        sup=false;
    }
    ofstream fout("convertido.txt");
    fout << cantidad<<"\n";

    int auxiliar=0;
    sup=false;
    vector<int> acumulados;
    int enl;

    for(int ii=0;ii<N;ii++){
        for(int jj=0;jj<t;jj++){
            if(ii==enlaces[jj][0] && !sup){
                sup=true;
                acumulados.push_back(enlaces[jj][2]);
            }
            if(ii==enlaces[jj][0] && sup){
                auxiliar++;
                acumulados.push_back(enlaces[jj][2]);
                enl=enlaces[jj][1];
            }
            if(jj==(t-1)){
                if(auxiliar!=0){
                    fout <<ii<<" "<<enl<<" "<<auxiliar<<" ";
                    for(int mm=1;mm<acumulados.size();mm++){
                        fout <<acumulados[mm]<<" ";
                    }
                    fout<<"\n";
                }
            }
        }
        auxiliar=0;
        sup=false;
        acumulados.clear();
    }

    fout.close();

    
    ifstream fin2("convertido.txt");
    fin2 >> T;
	  
    while(T--)   {
        fin2 >> X >> A >> Y;
        A++;
        for(i=0; i<Y; i++)  {
            fin2 >> j;
            EstadosNFA[X].transiciones[A][i] = j;
        }
    }
		
    fin2.close();

	  
    //Construye el DFA apartir del NFA
    D = 1;
    EstadosDFA.push_back(new EstadoDFA);
    EstadosDFA[0]->NFAEstadosConsistentes[0] = 1;
    ClausuraEpsilon(0, EstadosDFA[0]->NFAEstadosConsistentes);
    for(j=0; j<N; j++)
        if(EstadosDFA[0]->NFAEstadosConsistentes[j] == 1 && EstadosFinales_NFA.find(j) != EstadosFinales_NFA.end())  {
            EstadosDFA[0]->Estadofinal = true; EstadosFinales_DFA.push_back(0); break;
        }
    EstadosDFAIncompletos.push(0);
    while(!EstadosDFAIncompletos.empty()) {
        X = EstadosDFAIncompletos.front(); EstadosDFAIncompletos.pop();
        for(i=1; i<=M; i++)  {
            moverNFA(EstadosDFA[X]->NFAEstadosConsistentes, i, EstadosDFA[X]->transiciones[i]);
            ClausuraEpsilon(EstadosDFA[X]->transiciones[i], EstadosDFA[X]->transiciones[i]);
            for(j=0; j<D; j++)
                if(EstadosDFA[X]->transiciones[i] == EstadosDFA[j]->NFAEstadosConsistentes)  {
                   EstadosDFA[X]->TransicionSimbolica[i] = j;    break;
                }
            if(j == D)   {
                EstadosDFA[X]->TransicionSimbolica[i] = D;
                EstadosDFA.push_back(new EstadoDFA);
                EstadosDFA[D]->NFAEstadosConsistentes = EstadosDFA[X]->transiciones[i];
                for(j=0; j<N; j++)
                    if(EstadosDFA[D]->NFAEstadosConsistentes[j] == 1 && EstadosFinales_NFA.find(j) != EstadosFinales_NFA.end())  {
                        EstadosDFA[D]->Estadofinal = true; EstadosFinales_DFA.push_back(D); break;
                    }
                EstadosDFAIncompletos.push(D);
                D++;
            }
        }
    }

    // Escribe el DFA
    cout << "Estados\n";
    for(int i=0;i<D;i++){
        cout << i << "={";
            for(int n=0;n<N;n++){
                if(EstadosDFA[i]->NFAEstadosConsistentes[n]==1){
                    cout<<n<<" ";
                }
            }
        cout<< "}\n";
    }

    cout << "Estados de Aceptacion\n";
    for(vector<int>::iterator it=EstadosFinales_DFA.begin(); it!=EstadosFinales_DFA.end(); it++)
        cout<<*it<<" ";

    cout << "\n";
    cout << "Transiciones de Estados (x,y,z)\n";
    for(i=0; i<D; i++)  {
        for(j=1; j<=M; j++)
            cout << i << " " << j-1 << " " << EstadosDFA[i]->TransicionSimbolica[j] << "\n";
    }

    return 0;
}
