/*
  THIS FILE CONTAINS THE CODE FOR THE SECOND PASS OF THE ASSEMBLER.
  USES ONLY INTERMEDIATE FILE.
  PRODUCES LISTING FILE AND OBJECT FILE.

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
#include<climits>

using namespace std;

#include "Pass1.cpp"

ofstream obj,lst,mod;
ifstream intm;
int curr_block_num;

void modify_object_file()
{
   ifstream fin;
   fin.open("modification.txt");
   string s;
   while(true)
   {
       getline(fin,s);
       if(s=="") break;
       obj<<s<<endl;
   }
   //fin.close();
}

bool imm,ind;

void input(string a[])
{
    int i;
    for(i=0;i<6;++i)
        getline(intm,a[i]);
    cout<<"Input for line: "<<a[0]<<endl;
    for(i=1;i<6;++i)
        cout<<a[i]<<endl;
}

void assemble(string[]);
string gen_code(string[]);

string text_s="",text_e="";
int text_length=0,base=INT_MAX;

int main()
{
    run();
    string a[6];
    char ch;
    hexa start;
    if(error_flag)
    {
        cout<<"Errors encountered! Listing file not prepared!"<<endl;
        cout<<"Have a look at the error file to know more!"<<endl;
        exit(1);
    }
    intm.open("intermediate.txt");
    obj.open("object.txt");
    lst.open("list.txt");
    mod.open("modification.txt");
    lst<<"Line\tLoc   Block\t\tSource Statement\t\tObject Code"<<endl;
    lst<<"----------------------------------------------------------------"<<endl<<endl;
    input(a);
    curr_block="DEFAULT";
    curr_block_num=0;
    while(a[1]=="$")
    {
        lst<<a[0]<<"\t\t "<<a[2]<<endl;
        input(a);
    }
    if(a[2]=="START")
    {
        lst<<a[0]<<"\t"<<extendTo(4,a[4])<<"  "<<curr_block_num<<"\t\t"<<a[1]<<"\t\t"<<a[2]<<"\t\t"<<a[3]<<endl;
        obj<<"H^";
        int i;
        for(i=0;i<a[1].length();++i)
            obj<<a[1][i];
        for(;i<6;++i)
            obj<<" ";
        string temp=find_block(block_num-1);
        hexa len;
        len=toHex(toDec(BLOCK[temp].address)+toDec(BLOCK[temp].length));
        obj<<"^"<<extendTo(6,a[3])<<"^"<<extendTo(6,len)<<endl;
        start=a[3];
    }
    else
    {
        error_flag=1;
        error<<"Intermediate File has no start!"<<endl;
    }
    while(true)
    {
        input(a);
        if(a[1]=="$")
        {
            lst<<a[0]<<"\t\t "<<a[2]<<endl;
            continue;
        }
        if(a[2]=="END")
        {
            lst<<a[0]<<"\t\t\t\t\t\t"<<a[2]<<"\t\t"<<a[3]<<endl;
            if(text_length>0)
            {
                obj<<text_s<<"^"<<extendTo(2,toHex(text_length/2))<<text_e<<endl;
                cout<<"!!"<<endl;
            }
            text_length=0;
            text_s="";
            text_e="";
            modify_object_file();
            obj<<"E^"<<extendTo(6,start)<<endl;
            break;
        }
       // cout<<a[2]<<endl;
        assemble(a);
        cout<<"opcode: "<<a[2]<<":::";
       // cin>>ch;
    }
    if(error_flag)
    {
        cout<<"Errors encountered! Listing file not prepared!"<<endl;
        cout<<"Have a look at the error file to know more!"<<endl;
    }
    else
    {
        cout<<"INPUT FILE ASSEMBLED SUCCESSFULY!!"<<endl;
    }
}

void assemble(string a[])
{
    string object_code;
    hexa loc_ctr;
    int format;
    if(a[2]=="USE")
    {
        curr_block=a[3];
        curr_block_num=BLOCK[curr_block].num;
        lst<<a[0]<<"\t0000  "<<curr_block_num<<"\t\t\t"<<a[2]<<"\t\t"<<a[3]<<endl;
        if(text_length>0) obj<<text_s<<"^"<<extendTo(2,toHex(text_length/2))<<text_e<<endl;
        text_s="";
        text_e="";
        text_length=0;
        return;
    }
    if(a[2]=="RESB"||a[2]=="RESW")
    {
        lst<<a[0]<<"\t"<<extendTo(4,a[4])<<"  "<<curr_block_num<<"\t\t"<<a[1]<<"\t"<<a[2]<<"\t\t"<<a[3]<<endl;
        if(text_length>0) obj<<text_s<<"^"<<extendTo(2,toHex(text_length/2))<<text_e<<endl;
        text_s="";
        text_e="";
        text_length=0;
        return;
    }
    imm=ind=false;
    object_code=gen_code(a);
    cout<<"a[2]: "<<a[2]<<":::"<<object_code<<endl;
    if(a[2]=="BYTE"||a[2]=="WORD")
        lst<<a[0]<<"\t"<<extendTo(4,a[4])<<"  "<<curr_block_num<<"\t\t"<<a[1]<<"\t"<<a[2]<<"\t\t"<<a[3]<<endl;
    else
    {
        if(imm)      lst<<a[0]<<"\t"<<extendTo(4,a[4])<<"  "<<curr_block_num<<"\t\t"<<a[1]<<"\t\t"<<a[2]<<"\t\t#"<<a[3]<<"\t\t"<<object_code<<endl;
        else if(ind) lst<<a[0]<<"\t"<<extendTo(4,a[4])<<"  "<<curr_block_num<<"\t\t"<<a[1]<<"\t\t"<<a[2]<<"\t\t@"<<a[3]<<"\t\t"<<object_code<<endl;
        else         lst<<a[0]<<"\t"<<extendTo(4,a[4])<<"  "<<curr_block_num<<"\t\t"<<a[1]<<"\t\t"<<a[2]<<"\t\t"<<a[3]<<"\t\t"<<object_code<<endl;
    }
    if(text_s=="")
    {
        loc_ctr=toHex(toDec(BLOCK[curr_block].address)+toDec(a[4]));
        text_s="T^"+extendTo(6,loc_ctr);
        text_e="^"+object_code;
        text_length=object_code.length();
    }
    else if(text_length+object_code.length()>60)
    {
        obj<<text_s<<"^"<<extendTo(2,toHex(text_length/2))<<text_e<<endl;
        loc_ctr=toHex(toDec(BLOCK[curr_block].address)+toDec(a[4]));
        text_s="T^"+extendTo(6,loc_ctr);
        text_e="^"+object_code;
        text_length=object_code.length();
    }
    else
    {
        text_e+="^"+object_code;
        text_length+=object_code.length();
    }
    if(a[2]=="LDB")
    {
        base=toDec(SYMTAB[a[3]].address)+toDec(BLOCK[SYMTAB[a[3]].block].address);
    }
}

string gen_code(string a[])
{
    string ob1,ob2,ob3;
    hexa operand_addr,prgm_ctr;
    int format;
    if(a[2]=="BYTE")
    {
        int i;
        ob1="";
        if(a[3][0]=='X')
            for(i=2;i<a[3].length()-1;++i) ob1+=a[3][i];
        else //a[3][0]=='C'
            for(i=2;i<a[3].length()-1;++i)
                ob1+=toHex((int)a[3][i]);
        return ob1;
    }
    if(a[2]=="WORD")
    {
        ob1=toHex(atoi(a[3].c_str()));
        return ob1;
    }
    if(a[2]=="RSUB")
    {
        ob1="4F0000";
        return ob1;
    }
    if(a[2]=="+RSUB")
    {
        ob1="4F000000";
        return ob1;
    }
    if(a[2][0]=='+')
    {
        format=4;
        a[2]=a[2].substr(1);
    }
    else
        format=OPTAB[a[2]].format;
    if(format==1)
    {
        cout<<"Format 1"<<endl;
        ob1=OPTAB[a[2]].opcode;
        return ob1;
    }
    if(format==2)
    {
        cout<<"Format 2"<<endl;
        ob1=OPTAB[a[2]].opcode;
        if(a[3].length()==3)
        {
            ob2=toHex(reg_num(a[3][0]));
            if(isdigit(a[3][2]))  ob2=ob2+toHexDig(a[3][2]-1);
            else
            {
                ob2=ob2+toHexDig(reg_num(a[3][2]));
            }
        }
        else  //a[3].length==1
        {
            if(isdigit(a[3][0]))
            {
                ob2=toHex(atoi(a[3].c_str()))+"0";
                cout<<"isdigit! ob2: "<<ob2<<endl;
            }
            else
            {
                cout<<toHex(reg_num(a[3][0]))<<endl;
                ob2=toHex(reg_num(a[3][0]))+"0";
                cout<<"Not Isdigit! ob2: "<<ob2<<endl;
            }
        }
        cout<<"a[2]: "<<a[2]<<" ob1:"<<ob1<<"ob2:"<<ob2<<endl;
        return (ob1+extendTo(2,ob2));
    }
    if(format==3)
    {
        cout<<"Format 3"<<endl;
        cout<<a[2]<<endl;
        ob1=OPTAB[a[2]].opcode;
        if(a[3][0]=='#')
        {
            imm=true;
            cout<<"Immediate!"<<endl;
            ob1=toHex(toDec(ob1)+1);
            a[3]=a[3].substr(1);
            if(isdigit(a[3][0]))
            {
                ob2="0";
                ob3=toHex(atoi(a[3].c_str()));
                return extendTo(2,ob1)+ob2+extendTo(3,ob3);
            }
            //cout<<"ob1: "<<ob1<<endl;
        }
        else if(a[3][0]=='@')
        {
            ind=true;
            cout<<"Indirect!"<<endl;
            ob1=toHex(toDec(ob1)+2);
            a[3]=a[3].substr(1);
        }
        else
            ob1=toHex(toDec(ob1)+3);
        ob2="0";
        bool x=false;
        if(a[3][a[3].length()-2]==',')
        {
            x=true;
            ob2=toHex(toDec(ob2)+8);
            a[3]=a[3].substr(0,a[3].length()-2);
        }
        //cout<<"ob1:"<<ob1<<"ob2:"<<ob2<<endl;
        prgm_ctr=toHex(toDec(BLOCK[curr_block].address)+toDec(a[5]));
        operand_addr=toHex(toDec(SYMTAB[a[3]].address)+toDec(BLOCK[SYMTAB[a[3]].block].address));
        cout<<"prgm_ctr: "<<prgm_ctr<<" operand_addr: "<<operand_addr<<endl;
        if(x) a[3]+=",X";
        int disp=toDec(operand_addr)-toDec(prgm_ctr);
        cout<<"disp: "<<disp<<endl;
        if(disp>=-2048 && disp<2048)
        {
            ob2=toHex(toDec(ob2)+2);
            if(disp<0) disp+=4096;
            ob3=toHex(disp);
            return extendTo(2,ob1)+extendTo(1,ob2)+extendTo(3,ob3);
        }
        disp=toDec(operand_addr)-base;
        if(disp>=-2048 && disp<2048)
        {
            ob2=toHex(toDec(ob2)+4);
            if(disp<0) disp+=4096;
            ob3=toHex(disp);
            return extendTo(2,ob1)+extendTo(1,ob2)+extendTo(3,ob3);
        }
        //If still here, means overflow
        error_flag=1;
        error<<"Line "<<a[0]<<": Overflow detected"<<endl;
    }
    if(format==4)
    {
        ob1=OPTAB[a[2]].opcode;
        if(a[3][0]=='#')
        {
            imm=true;
            ob1=toHex(toDec(ob1)+1);
            a[3]=a[3].substr(1);
            if(isdigit(a[3][0]))
            {
                ob2="0";
                ob3=toHex(atoi(a[3].c_str()));
                a[2]="+"+a[2];
                return ob1+ob2+extendTo(5,ob3);
            }
        }
        else if(a[3][0]=='@')
        {
            ind=true;
            ob1=toHex(toDec(ob1)+2);
            a[3]=a[3].substr(1);
        }
        else
            ob1=toHex(toDec(ob1)+3);
        bool x=false;
        ob2="1";
        if(a[3][a[3].length()-2]==',')
        {
            x=true;
            ob2=toHex(toDec(ob2)+8);
            a[3]=a[3].substr(0,a[3].length()-2);
        }
        operand_addr=toHex(toDec(SYMTAB[a[3]].address)+toDec(BLOCK[SYMTAB[a[3]].block].address));
        if(x) a[3]+=",X";
        ob3=operand_addr;
        a[2]="+"+a[2];
        hexa loc_ctr=toHex(toDec(BLOCK[curr_block].address)+toDec(a[4])+1);
        mod<<"M^"<<extendTo(6,loc_ctr)<<"^05"<<endl;
        return extendTo(2,ob1)+extendTo(1,ob2)+extendTo(5,ob3);
    }
}
