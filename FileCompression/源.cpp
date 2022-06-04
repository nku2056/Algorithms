#include <iostream>
#include <fstream>
#include <cstring>
#include <queue>
#include<cmath>
using namespace std;

//存储每个哈夫曼树中字符的信息
class huffmanqueue
{
public:
	int weight;//出现次数
	char c;//字符
	string code;//字符编码
}letter[128], otherletter[128];//letter存储自己的文章 otherletter存储他人的文章

//存储哈夫曼树中每个节点的信息
class Node 
{
public:
	int weight;//出现次数
	char ch;//字符
	string code;//编码
	Node* lc, * rc;//左右孩子
	//构造函数
	Node()
	{
		lc = nullptr;
		rc = nullptr;
		code = "";
	}
	Node(int data)
	{
		lc = nullptr;
		rc = nullptr;
		weight = data;
		ch = NULL;
		code = "";
	}
	Node(int data, char c)
	{
		lc = nullptr;
		rc = nullptr;
		weight = data;
		ch = c;
		code = "";
	}
	Node(const Node& N)
	{
		ch = N.ch;
		weight = N.weight;
		code = N.code;
		if (N.lc)
		{
			lc = new Node();
			*lc = *(N.lc);
		}
		else
			lc = nullptr;
		if (N.rc)
		{
			rc = new Node();
			*rc = *(N.rc);
		}
		else
			rc = nullptr;
	}
	//为使用优先队列，重载小于号
	bool operator < (const Node& N) const
	{
		return weight > N.weight;
	}
};

//哈夫曼树的构造
class Huffman
{
public:
	Node* root;//自己文章哈夫曼树的根节点
	Node* otherroot = new Node();//他人文章哈夫曼树的根节点

	//构造函数
	Huffman()
	{
		root = nullptr;
	}

	//判断是否为根节点
	bool isleaf(Node* h)
	{
		if (h != nullptr && h->lc == nullptr && h->rc == nullptr)
			return true;
		else
			return false;
	}

	//建树（自己文章）
	void buildtree()
	{
		priority_queue<Node, vector<Node>, less<Node> > hfqueue;//优先队列
		
		//将每一个出现过的字符加入到优先队列中
		for (int i = 0; i < 128; i++)
		{
			if (letter[i].weight > 0)
			{
				Node* temp = new Node(letter[i].weight, letter[i].c);
				hfqueue.push(*temp);
			}
		}

		//从优先队列中取两个出现次数最少的节点，合并并放回队列
		while (hfqueue.size() > 1)
		{
			Node left = hfqueue.top();
			hfqueue.pop();
			Node right = hfqueue.top();
			hfqueue.pop();
			Node* parent = new Node(left.weight + right.weight);
			parent->lc = &left;
			parent->rc = &right;
			hfqueue.push(*parent);
		}

		//最后一个作为根节点
		root = new Node();
		*root = hfqueue.top();
		hfqueue.pop();
	}

	//根据编码表建树
	void buildothertree(string file1)
	{
		//读编码文件
		ifstream infile(file1);
		char c, a, b, e;
		int weight;
		string code;
		while (infile.get(c))
		{
			infile.get(a);
			infile >> weight;
			infile.get(b);
			infile >> code;
			infile.get(e);
			otherletter[(int)c].c = c;
			otherletter[(int)c].weight = weight;
			otherletter[(int)c].code = code;
		}

		//依照编码建树
		Node* temp = otherroot;
		for (int i = 0; i < 128; i++)
		{
			//字符出现过，则根据字符编码构造路径上的节点
			if (otherletter[i].weight > 0)
			{
				for (int j = 0; j < otherletter[i].code.size() - 1; j++)
				{
					if (otherletter[i].code[j] == '0')
					{
						if (temp->lc == NULL)
						{
							Node* left = new Node();
							temp->lc = left;
							temp = temp->lc;
						}
						else
						{
							temp = temp->lc;
						}
					}
					else
					{
						if (temp->rc == NULL)
						{
							Node* right = new Node();
							temp->rc = right;
							temp = temp->rc;
						}
						else
						{
							temp = temp->rc;
						}
					}
				}
				//叶节点处理
				if (otherletter[i].code[otherletter[i].code.size() - 1] == '0')
				{
					Node* left = new Node();
					left->ch = otherletter[i].c;
					left->code = otherletter[i].code;
					left->weight = otherletter[i].weight;
					temp->lc = left;
				}
				else
				{
					Node* right = new Node();
					right->ch = otherletter[i].c;
					right->code = otherletter[i].code;
					right->weight = otherletter[i].weight;
					temp->rc = right;
				}
				temp = otherroot;
			}
		}
		infile.close();
	}

	//销毁树
	void destorytree(Node* n)
	{
		if (n == nullptr)
		{
			return;
		}
		if (n->lc)
			destorytree(n->lc);
		if (n->rc)
			destorytree(n->rc);
	}

	//编码
	void buildcode(Node* n, string str,string codes)
	{
		fstream outputfile(codes, ios::app);
		if (isleaf(n))
		{
			//将编码存储到文件中
			n->code = str;
			letter[(int)n->ch].code = str;
			outputfile << n->ch << " " << n->weight << " " << n->code << endl;
			outputfile.close();
			return;
		}
		else
		{
			//对左孩子和右孩子递归
			if (n->lc)
			{
				buildcode(n->lc, str + '0',codes);
			}
			if (n->rc)
			{
				buildcode(n->rc, str + '1',codes);
			}
		}
	}
	//解码（他人压缩文件）
	void decodeother(string in_file,string out_file)
	{
		ifstream infile(in_file, ios::binary);//压缩文件
		ofstream outfile(out_file);//解压文件
		char c;
		bool flag = true;
		Node* temp = this->otherroot;
		string str = "";//存储编码
		//读压缩文件，每次一个字符
		while (infile.read(&c, sizeof(c)))
		{
			str += tostring(c);
			while (str.size() != 0)
			{
				//根据编码遍历
				while (!isleaf(temp))
				{
					if (str[0] == '0')
					{
						temp = temp->lc;
					}
					if (str[0] == '1')
					{
						temp = temp->rc;
					}
					str = str.substr(1);
					//如果编码为空则再读一个
					if (str.size() == 0)
					{
						if (infile.read(&c, sizeof(c)))
						{
							str += tostring(c);
						}
						else//空了
						{
							//最后一个字符读完后，若不是叶节点，则将flag设为false，不需要写入字符
							if (!isleaf(temp))
							{
								flag = false;
							}
							break;
						}
					}
				}
				if (flag)
				{
					outfile << temp->ch;
				}
				temp = this->otherroot;
			}
		}
		infile.close();
		outfile.close();
	}

	//解码（自己文章）原理同上
	void decode(string in_file,string out_file)
	{
		ifstream infile(in_file, ios::binary);
		ofstream outfile(out_file);
		char c;
		Node* temp = this->root;
		string str = "";
		bool flag = true;
		while (infile.read(&c, sizeof(c)))
		{
			str += tostring(c);
			while (str.size() != 0)
			{
				while (temp->lc != nullptr)
				{
					if (str[0] == '0')
					{
						temp = temp->lc;
					}
					if (str[0] == '1')
					{
						temp = temp->rc;
					}
					str = str.substr(1);
					if (str.size() == 0)
					{
						if (infile.read(&c, sizeof(c)))
						{
							str += tostring(c);
						}
						else
						{
							if (!isleaf(temp))
							{
								flag = false;
							}
							break;
						}
					}
				}
				if (flag)
				{
					outfile << temp->ch;
				}
				temp = this->root;
			}
		}
		infile.close();
		outfile.close();
	}

	//将一个压缩文件中存储的字符转为编码字符串
	string tostring(char c)
	{
		int a = c + 128;
		string str = "";
		//对2的幂取模，获取编码
		for (int i = 7; i >= 0; i--)
		{
			int p = a / (pow(2, i));
			if (p == 1)
			{
				str += "1";
			}
			else
			{
				str += "0";
			}
			a -= p * pow(2, i);
		}
		//在左侧补'0'
		for (int i = str.size(); i < 8; i++)
		{
			str.insert(0, 1, '0');
		}
		return str;
	}

	//将编码每8位存储为一个char型变量，对应到ASCII码为-128至127的char上
	char tochar(string str)
	{
		int a = 0;
		for (int i = 0; i < 8; i++)
		{
			a += pow(2, 7 - i) * (str[i] - '0');
		}

		char c = a - 128;
		return c;
	}

	//压缩过程
	void translate(string file1, string file2)
	{
		char c;
		string wordcode = "";
		fstream outfile(file1);
		fstream infile(file2, ios::out | ios::binary);
		//获取原文件中的每一个字符，将其根据编码进行压缩
		while (outfile.get(c))
		{
			wordcode += letter[(int)c].code;
			//长度够8，就进行一次压缩
			if (wordcode.size() >= 8)
			{
				string code8 = wordcode.substr(0, 8);
				char ch = tochar(code8);
				wordcode = wordcode.substr(8, wordcode.size() - 8);
				infile.write(&ch, sizeof(ch));
			}
		}
		//最后剩余的不足8，就找一个编码长度大于8的字符编码，补充到现编码之后，并只取8位进行压缩，这样在末尾不会产生冗余错误字符
		if (wordcode.size() != 0)
		{
			string str = "";
			for (int i = 0; i < 128; i++)
			{
				if (letter[i].weight > 0 && letter[i].code.size() >= 8)
				{
					str = letter[i].code;
					break;
				}
			}
			while (wordcode.size() < 8)
			{
				wordcode += str;
			}
			wordcode = wordcode.substr(0, 8);
			char ch = tochar(wordcode);
			infile.write(&ch, sizeof(ch));
		}
		outfile.close();
		infile.close();
	}
};

//读文章获取每一个字符的信息
void getfre(string file)
{
	for (int i = 0; i < 128; i++)
	{
		letter[i].weight = 0;
		letter[i].c = i;
		letter[i].code = "";
	}
	char a;
	fstream inputfile(file);
	while (!inputfile.eof())
	{
		a = inputfile.get();
		letter[(int)a].weight++;
	}
	inputfile.close();
}

int main()
{
	Huffman* huffman = new Huffman();
	getfre("my_text.txt");//获取字符信息
	huffman->buildtree();//建树（自己文章）
	huffman->buildcode(huffman->root, "","my_codes.txt");//编码（自己文章）
	huffman->translate("my_text.txt", "my_compression.dat");//压缩（自己文章）
	huffman->decode("my_compression.dat","my_decode_text.txt");//解码（自己文章）
	huffman->buildothertree("other_codes.txt");//建树（他人压缩文件）
	huffman->decodeother("other_compression.dat", "other_decode_text.txt");//解码（他人压缩文件）
	huffman->destorytree(huffman->root);
	huffman->destorytree(huffman->otherroot);//销毁，释放空间
	return 0;
}
