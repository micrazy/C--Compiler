/*************************************************************************
	> File Name: typeNode.h
	> Author: Micrazy
	> Mail: micrazy@live.com 
	> Created Time: Tue 19 Apr 2016 04:13:51 PM CST
 ************************************************************************/
typedef enum{
    false,true
}bool;
struct typeNode{
    bool istoken;
    int line;
    int firstcolumn,lastcolumn;
    char type[16];
    char text[32];
    struct typeNode* childNode;
    struct typeNode* brotherNode;
};
