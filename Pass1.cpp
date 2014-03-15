/*
  THIS FILE CONTAINS THE CODE FOR THE FIRST PASS OF THE ASSEMBLER.
  PRODUCES INTERMEDIATE FILE.

  @AUTHOR: Himanshu Jariyal
  B.TECH, CSE 2ND YEAR
  IIT ROORKEE
*/

#include<iostream>
#include<cstdlib>
#include<fstream>
#include<string>
#include<cstdio>
#include<map>

using namespace std;

#include "TABLES.cpp"
#include "HEXA_DEC.cpp"

bool isWhiteSpace(char a)
{
    if(a==' ')  return true;
    if(a=='\t') return true;
    return false;
}

void extract(string a,string word[],int& count)
{
    int i;
    for(i=0;i<5;++i) word[i]="";
    count=0;
    i=0;
    while(isWhiteSpace(a[i])&&i<a.length()) {++i;continue;}
    if(i==a.length()||a[i]=='.')   return;
    for(;i<a.length();)
    {
        while(isWhiteSpace(a[i])&&i<a.length()) {++i;continue;}
        if(i==a.length()) break;
        for(;!isWhiteSpace(a[i])&&i<a.length();++i) word[count]+=a[i];
        ++count;
    }
}

void execute(string[],int);

int block_num,line;
hexa pc;
string curr_block;
bool error_flag=0;


ifstream fin1;
ofstream fout1,error;

void run()
{
    create();
    char ch;
    string s,word[5];
    int count=0;
    fin1.open("input_fibonacci.txt");
    fout1.open("intermediate.txt");
    error.open("error.txt");
    line=5;
    getline(fin1,s);
    extract(s,word,count);
    while(count==0)
    {
        fout1<<line<<endl;
        fout1<<"$";
        fout1<<s<<endl;
        fout1<<""<<endl;
        fout1<<""<<endl;
        line+=5;
        cout<<"s: "<<s<<endl;
    }
    pc="0";
    BLOCK["DEFAULT"].num=0;
    BLOCK["DEFAULT"].address=pc;
    BLOCK["DEFAULT"].length="0";
    BLOCK["DEFAULT"].exist='y';
    curr_block="DEFAULT";
    block_num=1;
    line=5;
    if(word[0]=="START")
    {
        pc=word[1];
        fout1<<line<<endl;
        fout1<<""<<endl;
        fout1<<"START"<<endl;
        fout1<<pc<<endl;
        fout1<<pc<<endl;
        fout1<<""<<endl;
        cout<<"0 is start!"<<endl;
    }
    else if(word[1]=="START")
    {
        pc=word[2];
        fout1<<line<<endl;
        fout1<<word[0]<<endl;
        fout1<<"START"<<endl;
        fout1<<pc<<endl;
        fout1<<pc<<endl;
        fout1<<""<<endl;
        cout<<"1 is start!"<<endl;
    }
    else
        execute(word,count);
    while(true)
    {
        getline(fin1,s);
        extract(s,word,count);
        line+=5;
        cout<<"s: "<<s<<endl;
        fout1<<line<<endl;
        if(count==0)
        {
            cout<<"Comment detected!"<<endl;
            fout1<<"$"<<endl;
            fout1<<s<<endl;
            fout1<<""<<endl;
            fout1<<""<<endl;
            fout1<<""<<endl;
            continue;
        }
        if(word[0]=="END")
        {
            BLOCK[curr_block].length=pc;
            fout1<<""<<endl;
            fout1<<word[0]<<endl;
            fout1<<word[1]<<endl;
            fout1<<pc<<endl;
            fout1<<""<<endl;
            break;
        }
        execute(word,count);
       // cin>>ch;
     }
     hexa addr,len;
     string temp=find_block(0);
     addr=BLOCK[temp].address;
     len=BLOCK[temp].length;
     for(int i=1;i<block_num;++i)
     {
            temp=find_block(i);
            BLOCK[temp].address=toHex(toDec(addr)+toDec(len));
            addr=BLOCK[temp].address;
            len=BLOCK[temp].length;

     }
}

void execute(string word[],int count)
{
    cout<<"word[0]: "<<word[0]<<" pc: "<<pc<<endl;
    if(word[0]=="USE")
    {
        cout<<"USE detected!"<<endl;
        BLOCK[curr_block].length=pc;
        if(word[1]=="")
        {
            curr_block="DEFAULT";
            pc=BLOCK["DEFAULT"].length;
        }
        else if(BLOCK[word[1]].exist=='y')
        {
            curr_block=word[1];
            pc=BLOCK[word[1]].length;
        }
        else
        {
            BLOCK[word[1]].num=block_num;
            BLOCK[word[1]].exist='y';
            BLOCK[word[1]].length="0";
            curr_block=word[1];
            pc="0";
            ++block_num;
        }
        fout1<<""<<endl;
        fout1<<word[0]<<endl;
        fout1<<word[1]<<endl;
        fout1<<pc<<endl;
        fout1<<""<<endl;
        return;
    }
    if(word[0][0]=='+')
    {
        cout<<"Format 4"<<endl;
        fout1<<""<<endl;
        fout1<<word[0]<<endl;
        fout1<<word[1]<<endl;
        fout1<<pc<<endl;
        pc=toHex(toDec(pc)+4);
        fout1<<pc<<endl;
        return;
    }
    if(OPTAB[word[0]].exist=='y')
    {
        cout<<"0 is opcode!"<<endl;
        fout1<<""<<endl;
        fout1<<word[0]<<endl;
        fout1<<word[1]<<endl;
        fout1<<pc<<endl;
        pc=toHex(toDec(pc)+OPTAB[word[0]].format);
        fout1<<pc<<endl;
        return;
    }
    if(OPTAB[word[0]].exist=='n')
    {
        if(SYMTAB[word[0]].exist=='y')
        {
            error<<"Line "<<line<<": Duplicate Symbol"<<endl;
            error_flag=1;
        }
        else
        {
            SYMTAB[word[0]].address=pc;
            SYMTAB[word[0]].block=curr_block;
            SYMTAB[word[0]].exist='y';
            fout1<<word[0]<<endl;
            fout1<<word[1]<<endl;
            fout1<<word[2]<<endl;
            fout1<<pc<<endl;
            if(word[1][0]=='+')
                pc=toHex(toDec(pc)+4);
            else if(OPTAB[word[1]].exist=='y')
                pc=toHex(toDec(pc)+OPTAB[word[1]].format);
            else if(word[1]=="WORD")      pc=toHex(toDec(pc)+3);
            else if(word[1]=="RESW")      pc=toHex(toDec(pc)+(atoi(word[2].c_str())*3));
            else if(word[1]=="RESB")      pc=toHex(toDec(pc)+atoi(word[2].c_str()));
            else if(word[1]=="BYTE")
            {
                 int len=word[1].length()-3;
                 if(word[1][0]=='X') len/=2;
                 pc=toHex(toDec(pc)+len);
            }
            else
            {
                error<<"Line "<<line<<": Opcode not found"<<endl;
                error_flag=1;
            }
            fout1<<pc<<endl;
        }
    }
}


