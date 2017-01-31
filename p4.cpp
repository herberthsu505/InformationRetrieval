//Herbert Hsu
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>



using namespace std;

//returns the cosine similarity in doubles of two vectors
double CalculateCosSim(const vector<double>& f1, const vector<double>& f2)
{
	double x = 0;
	//dot product of two vectors
	for(unsigned int i = 0; i<f1.size(); i++)
	{
		x = x + f1.at(i)*f2.at(i);
	}
	double y = 0;
	double z = 0;
	//euclidean norms of the two vectors
	for(unsigned int i = 0; i<f1.size(); i++)
	{
		y = y + f1.at(i)*f1.at(i);
		z = z + f2.at(i)*f2.at(i);
	}
	return x/(sqrt(y)*sqrt(z));
}
//Returns index of word in dictionary, or -1 if not found
//word - string correpsonding to term in document (no spaces)
//dictionary - vector of vocabulary words (no duplicates)
int LookupWordInDictionary(string word, const vector<string>& dictionary)
{
	for(unsigned int i = 0; i<dictionary.size(); i++)
	{
		if(dictionary.at(i)==word)
			return i;
	}
	return -1;
}

//Converts string to lowercase.  Non-letter characters are unchanged.
//theString - arbitrary input string, output with all letters in lowercase
void ToLower(string& theString)
{
	for(unsigned int i = 0; i<theString.size(); i++)
	{
		theString.at(i)=tolower (theString.at(i));
	}

	return;
}
//Converts string to uppercase.  Non-letter characters are unchanged.
//theString - arbitrary input string, output with all letters in uppercase
void ToUpper(string& theString)
{
	for(unsigned int i = 0; i<theString.size(); i++)
	{
		theString.at(i)=toupper (theString.at(i));
	}

	return;
}

// TabulateTermFrequencies: Sets term frequency vector for document
//    document - string corresponding to a line of text from corpus file
//    dictionary - vector of vocabulary words (no duplicates)
//    termFreqs - output vector of calculated term frequences in document,
//                for all vocabulary words in dictionary

void TabulateTermFrequencies(string document, const vector<string>& dictionary, vector<double>& termFreqs) 
{  
	// initialize frequencies to zero
	for (unsigned int i=0; i < termFreqs.size(); i++) termFreqs.at(i) = 0;
	
	string term;
	int idx; 
	document.push_back('\n');   // terminate document with newline
                        // use stringstream to simplify parsing of doc into words
	istringstream docSS(document);  
	docSS >> term;
	while (docSS.good()) 
	{               
		ToLower(term);
		idx = LookupWordInDictionary(term,dictionary);
		if (idx >= 0)   // term found in dictionary
		  termFreqs.at(idx)++;
		docSS >> term;
	}
	return;
}
//build the vocabulary from input text file, assign number of times a word appear in the corpus
//also build a vector of documents where each element is a line of text(document)
void BuildDict(istream& file, vector<string>& dict, vector<int>& freq, vector<string>& corp)
{

	string line;
    while (getline(file, line))
	{
	    corp.push_back(line);
	    istringstream docSS(line);  
	    string word;
	    while (docSS>>word)
	    {
	        bool inDict = false;
	        ToLower(word);
	        for (unsigned int i=0; i<dict.size(); i++)
	        {
	           if (dict.at(i) == word)
	           {
	               inDict=true;
	               freq.at(i)++;
	           }
	        }
	        if(!inDict)
	        {
	            dict.push_back(word);
	            freq.push_back(1);
	        }
	    }
	}
    file.clear();
    return;
}
//buid idf vector
void BuildIdf(const vector<string>& corp, vector<double>& idf, const vector<int>& freq)
{
	for(unsigned int i = 0; i<idf.size(); i++)
	{
		idf.at(i) = log10( corp.size() / ((freq.at(i)+1)*1.0) );
	}
	return;
}
//hightlight the query in selected document
void HighlightQuery(string& doc, const string& query)
{
	istringstream docSS(query);  
	vector<string> queryWords;
    string word;
    string docLower = doc;
    ToLower(docLower);
    
    docLower = docLower + "                     												";
    while (docSS>>word)
    {
    	ToLower(word);
    	string upperWord= word;
    	ToUpper(upperWord);
    	for(unsigned int i = 0; i<doc.size(); i++)
    	{
    		int count = 0;
    		for(unsigned int j = 0; j<word.size(); j++)
    		{
    			if(docLower.at(i+j)==word.at(j))
    				count++;
    		}
    		if(count==word.size() && docLower.at(i+word.size())==' ')
    		{
    			doc.replace(i, word.size(),upperWord);
    		}
    	}
    }
    return;
}
int main()
{
	ifstream in1, in2; 
	string fileName, query, line;
	int n;
	vector<string> vocabDict;
	vector<string> corpus;
	vector<int> frequency;
	getline( cin, fileName);
    getline( cin, query);	
	in1.open(fileName.c_str()); //open file
	BuildDict(in1,vocabDict,frequency,corpus); //build dictionary
	vector<double> tfQuery(vocabDict.size(),0);	
	vector<double> fQuery(vocabDict.size(),0);	
	n = corpus.size(); //number of documents
	vector<int> docFreq(vocabDict.size());
	vector<double> iDF(vocabDict.size());
	BuildIdf(corpus, iDF, frequency); //build the idf vector

	//build tf vector for query
	TabulateTermFrequencies(query, vocabDict, tfQuery);
	//build idf-tf vector for query
	for(unsigned int i = 0; i<tfQuery.size(); i++)
	{
		fQuery.at(i) = tfQuery.at(i)*iDF.at(i);
	}
	double max = 0;
	int maxDoc = 0;
	for(int i =0; i<n; i++)
	{
		vector<double> tFDoc(vocabDict.size());
		vector<double> featurnVDoc(vocabDict.size());
		//build tf vector for document
		TabulateTermFrequencies(corpus.at(i), vocabDict, tFDoc);
		//build idf-tf vector for document
		for(unsigned int j = 0; j<vocabDict.size(); j++)
		{
			featurnVDoc.at(j) = tFDoc.at(j)*iDF.at(j);
		}
		//find cosine similarity
		double cosSim = CalculateCosSim(featurnVDoc, fQuery);
		//find max cosine similarity
		if(cosSim>max)
		{
			max=cosSim;
			maxDoc=i;
		}
	}
	//if no match
    if(max==0)
    {
    	cout<<endl;
    }
    //best match
    else
    {
	 	HighlightQuery(corpus.at(maxDoc), query); //highlight keywords
	 	cout<< corpus.at(maxDoc)<< endl;
	}

	return 0;    
}