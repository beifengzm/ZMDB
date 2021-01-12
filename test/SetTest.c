#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Set.h"
#include "Hash.h"

#define N 5

int main()
{
    struct Set** pSets = (struct Set**)malloc(sizeof(struct Set*)*N);
    for (int i = 0; i < 5; ++i)
    {
        pSets[i] = newSet();
    }

    char* strs[N][7] = 
    {
        {"nihao","luxiaofeng","alibaba","zhangsan", "lisi", "zhangsan", "bbb"},
        {"nihao","xiaowang", "bbb", "lisi", "zhangsan", "luxiaofeng", "alibaba"},
        {"nihao","alibaba", "bbb", "lisi", "zhangsan", "aaa", "ccc"},
        {"alibaba", "bbb", "lisi", "zhangsan", "huauque", "nihao", "ddd"},
        {"tengxun", "bbb", "lisi", "zhangsan", "alibaba", "nihao", "eee"}
    };

    for (int i = 0; i < N; ++i)
    {
        int n = sizeof(strs[i])/sizeof(char*);
        for (int j = 0; j < n; ++j)
            addSet(pSets[i], strs[i][j]);
    }

    struct Set* pSetInter = interSets(pSets, N);
    struct Set* pSetUnion = unionSets(pSets, N);
    
    char* result;
    int i = 1;
    printf("Intersection(%d): \n", sizeSet(pSetInter));
    while (iterateSet(pSetInter, (void**)&result))
    {
        printf("%d) %s\n", i, result);
        i++;
    }

    i = 1;
    printf("Union(%d):\n", sizeSet(pSetUnion));
    while (iterateSet(pSetUnion, (void**)&result))
    {
        printf("%d) %s\n", i, result);
        i++;
    }
    
    // freeSet(pSets);
    // freeSet(pSetInter);
    // freeSet(pSetUnion);
    return 0;
}