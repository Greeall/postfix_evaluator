// postfix_evaluator.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>

using namespace std;
struct function;



string token;          
vector <string> stack;
string prompt = "--> ";
bool will_change_prompt = false;
vector <function> functions;

struct function 
{
  string name;
  vector<string> body;
};

bool is_fname_number_or_built_in_function(string& t)
{
	bool yn = false;

	if(t == "+" || t == "-" || t == "*" || t == "/" || 
	   t == "." || t == "inc" || t == "dec" || t == "dup" ||
	   t == "pstack" || t == "%" || t == "^2" || t == "pop" ||
	   t == "popn" || t == "dupn" || t == "swap" ||
	   t == "size" || t == "clear" || t == "prompt" ||
	   t == "sin" || t == "cos" || t == "def" ||
	   t == "functions" || t == ";")
	   yn = true;

	istringstream iss(t);
	double x;
	if (iss >> x)
		yn = true;

	for(int i = 0; i < functions.size(); i ++)
		if(t == functions[i].name)
			yn = true;
	
	return yn;
}

void print(const vector<string>& v)
{
	const int n = v.size();
	if (n == 0) ;   // print nothing for an empty vector
	else if (n == 1) 
		cout << v[0];
	else
	{          // n >= 2
		cout << v[0];
		for(int i = 1; i < v.size(); ++i) // note that i starts at 1
			cout << " " << v[i];
	}
}

void print(function f) 
{
	cout << f.name << ": [";
	print(f.body);
	cout << "]";
}




void error(string& err)
{
	cout<<err;
}

double string_to_double(const string& s) 
{
	istringstream iss(s);
	double x;
	if (!(iss >> x))
		 error("Error: can't convert \"" + s + "\" to a double\n");
	return x;
}

void chang_prompt()
{
	prompt = token;
	will_change_prompt = false;
}

double peek() 
{
	if (stack.size() == 0)
		error("Error: " + token + " tried to peek at an empty stack\n");
	else
		return string_to_double(stack.back());
}

double pop()
{
	double tos = peek();
	stack.pop_back();
	return tos;
}

void push(string& a)
{
	stack.push_back(a);
}

void print_stack()
{
  const int n = stack.size();
  if (n == 0) 
	  cout << "<empty stack>";
  else if (n == 1) 
	  cout << "<tos=" << stack[0] << ">";
  else 
  { 
    cout << "<tos=";
    for(int i = n - 1; i >= 0; --i)
      cout << ", " << stack[i];
    cout << ">";
  }
  cout << endl;
}

void pop_command()
{
	if(stack.size() == 0)
		cout << "<ERROR: stack already empty>\n";
	else if (stack.size() == 1)
	{
		cout << "<stack empty>\n";
		cout << prompt;
		pop();
	}
	else
		pop();	
}

void pop_n_elements()
{
	int n = pop();

	if(n > stack.size()) 
		n = stack.size();

	if(n == 0) 
	{
		cout << "<ERROR: stack was empty>\n";
		cout << prompt;
	}

	for(int i = 0; i < n; i ++)
		pop_command();
}

void process_of_division()
{
	double b, a;
	bool is_float = false;

	if (stack.size() == 0)
		error("Error: " + token + " tried to peek at an empty stack\n");

	string b_string = stack.back();
	stack.pop_back();

	for(int i = 0; i < b_string.size(); i ++)
		if(b_string[i] == '.')
		{
			b = string_to_double(b_string);
			is_float = true;
			break;
		}
		else
			b = string_to_double(b_string);
			
	if (stack.size() == 0)
		error("Error: " + token + " tried to peek at an empty stack\n");		

	string a_string = stack.back();
	stack.pop_back();

	for(int i = 0; i < a_string.size(); i ++)
		if(a_string[i] == '.')
		{
			a = string_to_double(a_string);
			is_float = true;
			break;
		}
		else
			a = string_to_double(a_string);
			
		
	
	if(is_float)
	{
		float division = a/b;
		push(to_string(division));
	}
	else
	{
		int division = a/b;
		push(to_string(division));
	}
	
}

void process_token() 
{
	if (token == "+")
	{
		double b = pop();
		double a = pop();
		
		push(to_string(a + b));
	} 
	else if (token == "-")
	{
		double b = pop();
		double a = pop();
		push(to_string(a - b));
	} 
	else if (token == "*")
	{
		double b = pop();
		double a = pop();
		push(to_string(a * b));
	}
	else if (token == "/")
	{
		process_of_division();
	}
	else if (token == ".")
	{
		if (stack.size() == 0)
		{
			print_stack();
			cout << prompt;
		}
		else
		{
			cout << "tos = " <<  string_to_double(stack[stack.size() - 1]) << "\n";
			cout << prompt;
		}
	}
	else if (token == "inc")
	{
		double b = pop();
		push(to_string(b + 1));
	}
	else if (token == "dec")
	{
		double b = pop();
		push(to_string(b - 1));
	}
	else if (token == "dup")
	{
		double b = peek();
		push(to_string(b));
	}
	else if (token == "pstack")
	{
		print_stack();
	}
	else if (token == "%")
	{
		int b = pop();
		int a = pop();
		push(to_string(a % b));
	}
	else if (token == "^2")
	{
		double a = pop();
		push(to_string(a*a));
	}
	else if (token == "pop")
	{
		pop_command();
	}
	else if (token == "popn")
	{
		pop_n_elements();
	}
	else if (token == "dupn")
	{
		int n = pop();

		if(n < 0) 
			cout<< "ERROR: "<< n <<  " less than zero";

		int i = stack.size() - n;
		int max = stack.size();

		for(i; i < max; i ++)
			push(stack[i]);
	}
	else if(token == "swap")
	{
		int first = pop();
		int second = pop();
		push(to_string(first));
		push(to_string(second));
	}
	else if(token == "size")
	{
		int size = stack.size();
		push(to_string(size));
	}
	else if(token == "clear")
	{
		stack.clear();
	}
	else if(token == "prompt")
	{
		will_change_prompt = true;
	}
	else if(token == "sin")
	{
		double arg = pop();
		push(to_string(sin(arg)));
	}
	else if(token == "cos")
	{
		double arg = pop();
		push(to_string(cos(arg)));
	}
	else if(token == "def")
	{
		function f;
		cin >> f.name;  // read the name

		
		if(is_fname_number_or_built_in_function(f.name)) 
		{
			cout << "Error: you cann't use \"" << f.name <<"\" as name for function" << endl;
			cout << prompt;
			return;
		}		
		

		string eq;
		cin >> eq;      // read the "="

		string t;
		cin >> t;

		while (t != ";") 
		{
			if(t == "def" || t == "." || t == eq || t == f.name);
			else
				f.body.push_back(t);
			cin >> t;
		}

		functions.push_back(f);

		print(f);
		cout << endl << prompt;

	}
	else if(token == "functions")
	{
		if(functions.size() == 0) cout << "There aren't any functions."<< endl << prompt;
		for(int i = 0; i < functions.size(); i ++)
		{
			print(functions[i]);
			cout << endl;
			cout << prompt;
		}
	}
	else
	{

		for(int i = 0; i < functions.size(); ++i)
		{
			function f = functions[i];

			if (token == f.name)
			{
        // found a matching user-defined function, so execute it
        // by processing each token in its body
				for(int j = 0; j < f.body.size(); ++j)
				{
					token = f.body[j];
					process_token();  
				}
				return;  
			}
		}

    // we only get to this line if token matches nothing in the
    // functions vector: we assume token is a number


		push(token);
	}	
}


int _tmain(int argc, _TCHAR* argv[])
{

	cout << "Postfix expression evaluator, version 1\n";
	cout << prompt;

	 while (cin >> token)
	 {
		try
		{
			(will_change_prompt) ? chang_prompt() : process_token();
		}
		catch (runtime_error e)
		{
			cout << e.what();  // print the error message
			cout << prompt;
		} 
	 }

	return 0;
}

