# Trabalho2_OrganizacaoDeArquivos
Repositório privado para o controle do versionamento da implementação do trabalho 2 da disciplina SCC0215 - Organização de Arquivos.

## Objetivos
O objetivo principal é ser capaz de implementar um sistema que lida com dados presentes em arquivos binários, simulando as funcionalidades implementadas na linguagem SQL, como inserção, busca, remoção, entre outras...
### No escopo desse trabalho
Referindo-se à localidade desse trabalho, cade ao programa ser capaz de implementar as funções Create_index, Select_from_where e insert_into, agora baseados em um método de idexação mais eficiente e condizente à redução de acesso à paginas de disco.

## Contexto
O escopo utilizado é uma base de dados simulada que representa crimes. Tais registros constam com um número de identificação, o lugar de ocorrência, o número do artigo da infração, a descrição do crime, a data de ocorrência, a marca do celular - em caso de roubo ou furto. Tais dados podem ser nulos ou não, com excessão do número de identificação, que será utilizado como referência para a indexação

## Implementação
Para atingir tal marca, o programa utilizará a estrtura de dados complexa Árvore-B*, que representa uma árvore cujos nós representam uma página de disco e sua eficiência é equivalente à altura da árvore respectiva. 
Sobre tal escopo, as árvores contarão com ordem 5, isto é, um nó-página cheio terá 5 chaves descendentes. Lembrando que a árvore b* garante ocupação mínima de 86%
### Paginação
O conceito de páginas de disco será lógicamente aplicado. E, sobre a luz da disciplina, o número escolhido para o tamanho dessa unidade de memória secundária será de 76 bytes.

## Links para estudos
[Introdução à árvore B - geeks4geeks](https://www.geeksforgeeks.org/introduction-of-b-tree-2/)

[Inserção árvore B - geeks4geeks](https://www.geeksforgeeks.org/insert-operation-in-b-tree/)

[B* em c++ - tutorialsPoint](https://www.tutorialspoint.com/b-trees-implementation-in-cplusplus) - achei a fonte meio duvidosa aqui

[Git sobre B* - ta na net](https://github.com/eapacheco/bstar-tree)

[Git sobre B - peguei do bucke](https://github.com/falcaopetri/B-Tree)

[B* - geeks4geeks](https://www.geeksforgeeks.org/b-trees-implementation-in-c/)
