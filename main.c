//
// Created by 孙健淞 on 25-6-7.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hfm.h"


int main() {
    char choice;
    do {
        printf("\nI:Initialize\nE:Encode\nD:Decode\nP:Print Codes\nT:Print Tree\nQ:Quit\nChoose: ");
        scanf(" %c", &choice);
        switch (choice)
        {
        case 'I': Initialize(); break;
        case 'E': Encode(); break;
        case 'D': Decode(); break;
        case 'P': Print();  break;
        case 'T': Treeprint(); break;
        case 'Q': break;
        default: printf("Invalid choice.\n");
        }
    } while (choice != 'Q');
    return 0;
}