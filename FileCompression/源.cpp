#include <iostream>
#include <fstream>
#include <cstring>
#include <queue>
#include<cmath>
using namespace std;

//�洢ÿ�������������ַ�����Ϣ
class huffmanqueue
{
public:
	int weight;//���ִ���
	char c;//�ַ�
	string code;//�ַ�����
}letter[128], otherletter[128];//letter�洢�Լ������� otherletter�洢���˵�����

//�洢����������ÿ���ڵ����Ϣ
class Node 
{
public:
	int weight;//���ִ���
	char ch;//�ַ�
	string code;//����
	Node* lc, * rc;//���Һ���
	//���캯��
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
	//Ϊʹ�����ȶ��У�����С�ں�
	bool operator < (const Node& N) const
	{
		return weight > N.weight;
	}
};

//���������Ĺ���
class Huffman
{
public:
	Node* root;//�Լ����¹��������ĸ��ڵ�
	Node* otherroot = new Node();//�������¹��������ĸ��ڵ�

	//���캯��
	Huffman()
	{
		root = nullptr;
	}

	//�ж��Ƿ�Ϊ���ڵ�
	bool isleaf(Node* h)
	{
		if (h != nullptr && h->lc == nullptr && h->rc == nullptr)
			return true;
		else
			return false;
	}

	//�������Լ����£�
	void buildtree()
	{
		priority_queue<Node, vector<Node>, less<Node> > hfqueue;//���ȶ���
		
		//��ÿһ�����ֹ����ַ����뵽���ȶ�����
		for (int i = 0; i < 128; i++)
		{
			if (letter[i].weight > 0)
			{
				Node* temp = new Node(letter[i].weight, letter[i].c);
				hfqueue.push(*temp);
			}
		}

		//�����ȶ�����ȡ�������ִ������ٵĽڵ㣬�ϲ����Żض���
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

		//���һ����Ϊ���ڵ�
		root = new Node();
		*root = hfqueue.top();
		hfqueue.pop();
	}

	//���ݱ������
	void buildothertree(string file1)
	{
		//�������ļ�
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

		//���ձ��뽨��
		Node* temp = otherroot;
		for (int i = 0; i < 128; i++)
		{
			//�ַ����ֹ���������ַ����빹��·���ϵĽڵ�
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
				//Ҷ�ڵ㴦��
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

	//������
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

	//����
	void buildcode(Node* n, string str,string codes)
	{
		fstream outputfile(codes, ios::app);
		if (isleaf(n))
		{
			//������洢���ļ���
			n->code = str;
			letter[(int)n->ch].code = str;
			outputfile << n->ch << " " << n->weight << " " << n->code << endl;
			outputfile.close();
			return;
		}
		else
		{
			//�����Ӻ��Һ��ӵݹ�
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
	//���루����ѹ���ļ���
	void decodeother(string in_file,string out_file)
	{
		ifstream infile(in_file, ios::binary);//ѹ���ļ�
		ofstream outfile(out_file);//��ѹ�ļ�
		char c;
		bool flag = true;
		Node* temp = this->otherroot;
		string str = "";//�洢����
		//��ѹ���ļ���ÿ��һ���ַ�
		while (infile.read(&c, sizeof(c)))
		{
			str += tostring(c);
			while (str.size() != 0)
			{
				//���ݱ������
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
					//�������Ϊ�����ٶ�һ��
					if (str.size() == 0)
					{
						if (infile.read(&c, sizeof(c)))
						{
							str += tostring(c);
						}
						else//����
						{
							//���һ���ַ������������Ҷ�ڵ㣬��flag��Ϊfalse������Ҫд���ַ�
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

	//���루�Լ����£�ԭ��ͬ��
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

	//��һ��ѹ���ļ��д洢���ַ�תΪ�����ַ���
	string tostring(char c)
	{
		int a = c + 128;
		string str = "";
		//��2����ȡģ����ȡ����
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
		//����ಹ'0'
		for (int i = str.size(); i < 8; i++)
		{
			str.insert(0, 1, '0');
		}
		return str;
	}

	//������ÿ8λ�洢Ϊһ��char�ͱ�������Ӧ��ASCII��Ϊ-128��127��char��
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

	//ѹ������
	void translate(string file1, string file2)
	{
		char c;
		string wordcode = "";
		fstream outfile(file1);
		fstream infile(file2, ios::out | ios::binary);
		//��ȡԭ�ļ��е�ÿһ���ַ���������ݱ������ѹ��
		while (outfile.get(c))
		{
			wordcode += letter[(int)c].code;
			//���ȹ�8���ͽ���һ��ѹ��
			if (wordcode.size() >= 8)
			{
				string code8 = wordcode.substr(0, 8);
				char ch = tochar(code8);
				wordcode = wordcode.substr(8, wordcode.size() - 8);
				infile.write(&ch, sizeof(ch));
			}
		}
		//���ʣ��Ĳ���8������һ�����볤�ȴ���8���ַ����룬���䵽�ֱ���֮�󣬲�ֻȡ8λ����ѹ����������ĩβ���������������ַ�
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

//�����»�ȡÿһ���ַ�����Ϣ
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
	getfre("my_text.txt");//��ȡ�ַ���Ϣ
	huffman->buildtree();//�������Լ����£�
	huffman->buildcode(huffman->root, "","my_codes.txt");//���루�Լ����£�
	huffman->translate("my_text.txt", "my_compression.dat");//ѹ�����Լ����£�
	huffman->decode("my_compression.dat","my_decode_text.txt");//���루�Լ����£�
	huffman->buildothertree("other_codes.txt");//����������ѹ���ļ���
	huffman->decodeother("other_compression.dat", "other_decode_text.txt");//���루����ѹ���ļ���
	huffman->destorytree(huffman->root);
	huffman->destorytree(huffman->otherroot);//���٣��ͷſռ�
	return 0;
}
