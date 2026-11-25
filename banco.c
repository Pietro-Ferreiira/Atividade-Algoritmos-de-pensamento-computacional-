#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Cada cliente guarda 3 contas: posição 0 = corrente (01),
   posição 1 = salário (02), posição 2 = poupança (03) */
struct Cliente {
    char nome[50];
    int base;           /* número da conta: 1000, 1001, ... */
    int tem[3];         /* 1 se tem a conta, 0 se não tem */
    double saldo[3];    /* saldo de cada conta */
    int titulo;         /* 1 se já comprou título */
    int emprestimo;     /* 1 se já pegou empréstimo */
};

struct Cliente clientes[100];
int qtd = 0;            /* quantidade de clientes */
int bancoAberto = 0;    /* o banco começa fechado */
int ganhador = -1;      /* posição do ganhador (-1 = ninguém) */
double premio = 0;

/* soma o saldo de todas as contas do cliente */
double total(int i) {
    double soma = 0;
    for (int t = 0; t < 3; t++) {
        soma = soma + clientes[i].saldo[t];
    }
    return soma;
}

/* avisa se a conta corrente estiver negativa */
void alerta(int i) {
    if (clientes[i].tem[0] == 1 && clientes[i].saldo[0] < 0) {
        printf("ALERTA: %s esta com saldo negativo na conta corrente!\n", clientes[i].nome);
    }
}

/* verifica se o cliente pode pegar empréstimo */
int aptoEmprestimo(int i) {
    return clientes[i].tem[2] == 1 && clientes[i].saldo[2] > 0
        && clientes[i].tem[0] == 1 && clientes[i].emprestimo == 0;
}

/* verifica se o cliente pode comprar título */
int aptoTitulo(int i) {
    return clientes[i].tem[1] == 1 && clientes[i].saldo[1] >= 1000
        && clientes[i].saldo[1] <= 1500 && clientes[i].titulo == 0;
}

void abrirBanco() {
    int n, ok;
    if (bancoAberto == 1) {
        printf("O banco ja esta aberto.\n");
        return;
    }
    printf("Quantos clientes? ");
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        struct Cliente *c = &clientes[qtd];
        printf("Nome do cliente (sem espacos): ");
        scanf("%49s", c->nome);
        do {
            ok = 1;
            printf("Tem conta corrente (01)? 1-sim 0-nao: ");
            scanf("%d", &c->tem[0]);
            printf("Tem conta salario (02)? 1-sim 0-nao: ");
            scanf("%d", &c->tem[1]);
            printf("Tem conta poupanca (03)? 1-sim 0-nao: ");
            scanf("%d", &c->tem[2]);
            if (c->tem[0] == 1 && c->tem[1] == 1) {
                printf("Nao pode ter conta salario e corrente juntas.\n");
                ok = 0;
            } else if (c->tem[0] == 0 && c->tem[1] == 0 && c->tem[2] == 0) {
                printf("O cliente precisa ter pelo menos uma conta.\n");
                ok = 0;
            }
        } while (ok == 0);
        c->base = 1000 + qtd;
        for (int t = 0; t < 3; t++) {
            c->saldo[t] = 0;    /* tudo começa zerado */
        }
        c->titulo = 0;
        c->emprestimo = 0;
        qtd++;
    }
    bancoAberto = 1;
    for (int i = 0; i < qtd; i++) {
        alerta(i);
    }
    printf("Banco aberto!\n");
}

void consultarHistorico() {
    double soma = 0;
    for (int i = 0; i < qtd; i++) {
        soma = soma + total(i);
    }
    printf("\n== Saldo Total do banco ==\n");
    printf("%d | R$ %.2f\n", qtd, soma);

    printf("== Clientes Aptos a Emprestimos ==\n");
    for (int i = 0; i < qtd; i++) {
        if (aptoEmprestimo(i)) {
            printf("%s|%d-01|%.2f\n", clientes[i].nome, clientes[i].base, 0.25 * 10 * total(i));
        }
    }

    printf("== Clientes com Saldo Negativo na Conta Corrente ==\n");
    for (int i = 0; i < qtd; i++) {
        if (clientes[i].tem[0] == 1 && clientes[i].saldo[0] < 0) {
            printf("%s|%d-01|%.2f\n", clientes[i].nome, clientes[i].base, clientes[i].saldo[0]);
        }
    }

    printf("== Clientes Aptos a Compra de Titulos ==\n");
    for (int i = 0; i < qtd; i++) {
        if (aptoTitulo(i)) {
            printf("%s|%d-02|%.2f\n", clientes[i].nome, clientes[i].base, clientes[i].saldo[1]);
        }
    }

    printf("== Clientes Contemplado no Sorteio ==\n");
    if (ganhador != -1) {
        printf("%s|%d-02|%.2f\n", clientes[ganhador].nome, clientes[ganhador].base, premio);
    }
}

void efetuarSorteio() {
    int ordem[100], aux;
    if (qtd < 5) {
        printf("Precisa de pelo menos 5 clientes.\n");
        return;
    }
    /* ordena as posições do maior para o menor saldo */
    for (int i = 0; i < qtd; i++) {
        ordem[i] = i;
    }
    for (int i = 0; i < qtd - 1; i++) {
        for (int j = 0; j < qtd - 1 - i; j++) {
            if (total(ordem[j]) < total(ordem[j + 1])) {
                aux = ordem[j];
                ordem[j] = ordem[j + 1];
                ordem[j + 1] = aux;
            }
        }
    }
    /* os 5 primeiros precisam ter título */
    for (int i = 0; i < 5; i++) {
        if (clientes[ordem[i]].titulo == 0) {
            printf("Sorteio cancelado: %s nao comprou titulo.\n", clientes[ordem[i]].nome);
            return;
        }
    }
    /* Fibonacci: 1, 1, 2, 3, 5 */
    int fib[5] = {1, 1};
    for (int i = 2; i < 5; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];   /* soma os dois anteriores */
    }
    /* o 1º maior saldo recebe peso 5, o 2º peso 3, ... (fib de trás para frente) */
    int soma = 0;
    for (int i = 0; i < 5; i++) {
        soma = soma + fib[i];               /* 1+1+2+3+5 = 12 */
    }
    int sorteado = rand() % soma;           /* número de 0 a 11 */
    int acumulado = 0;
    int escolhido = ordem[4];
    for (int i = 0; i < 5; i++) {
        acumulado = acumulado + fib[4 - i];
        if (sorteado < acumulado) {
            escolhido = ordem[i];
            break;
        }
    }
    ganhador = escolhido;
    premio = 10 * clientes[ganhador].saldo[1];   /* 10x o saldo da conta salário */
    clientes[ganhador].saldo[1] += premio;
    printf("Vencedor: %s | Premio: R$ %.2f\n", clientes[ganhador].nome, premio);
}

void operacoes() {
    int n, op, tipo;
    double valor;
    for (int i = 0; i < qtd; i++) {
        printf("%d - %s\n", i + 1, clientes[i].nome);
    }
    printf("Numero do cliente: ");
    scanf("%d", &n);
    if (n < 1 || n > qtd) {
        printf("Cliente invalido.\n");
        return;
    }
    n = n - 1;
    alerta(n);
    printf("Saldos -> corrente: %.2f | salario: %.2f | poupanca: %.2f\n",
           clientes[n].saldo[0], clientes[n].saldo[1], clientes[n].saldo[2]);
    printf("1-Depositar 2-Sacar 3-Emprestimo 4-Comprar titulo: ");
    scanf("%d", &op);

    if (op == 1 || op == 2) {
        printf("Qual conta (1-corrente, 2-salario, 3-poupanca)? ");
        scanf("%d", &tipo);
        if (tipo < 1 || tipo > 3 || clientes[n].tem[tipo - 1] == 0) {
            printf("Cliente nao tem essa conta.\n");
            return;
        }
        tipo = tipo - 1;
        printf("Valor: ");
        scanf("%lf", &valor);
        if (op == 1) {
            clientes[n].saldo[tipo] += valor;
        } else if (tipo != 0 && valor > clientes[n].saldo[tipo]) {
            printf("Saldo insuficiente.\n");
        } else {
            clientes[n].saldo[tipo] -= valor;
        }
    } else if (op == 3) {
        if (aptoEmprestimo(n)) {
            valor = 0.25 * 10 * total(n);
            clientes[n].saldo[0] += valor;
            clientes[n].emprestimo = 1;
            printf("Emprestimo concedido: %.2f\n", valor);
        } else {
            printf("Cliente nao esta apto.\n");
        }
    } else if (op == 4) {
        if (aptoTitulo(n)) {
            clientes[n].titulo = 1;
            printf("Titulo adquirido!\n");
        } else {
            printf("Precisa ter entre 1000 e 1500 na conta salario.\n");
        }
    }
}

int main() {
    int opcao;
    srand(time(NULL));      /* faz o sorteio mudar a cada execução */
    do {
        printf("\n1 - Abrir o banco\n");
        printf("2 - Consultar historico\n");
        printf("3 - Efetuar sorteio\n");
        printf("4 - Sair\n");
        printf("5 - Operacoes do cliente\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            abrirBanco();
        } else if (opcao == 4) {
            printf("Encerrando...\n");
        } else if (opcao >= 2 && opcao <= 5) {
            if (bancoAberto == 0) {
                printf("Abra o banco primeiro!\n");
            } else if (opcao == 2) {
                consultarHistorico();
            } else if (opcao == 3) {
                efetuarSorteio();
            } else {
                operacoes();
            }
        } else {
            printf("Opcao invalida.\n");
        }
    } while (opcao != 4);
    return 0;
}
