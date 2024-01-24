#include<string>
#include<math.h>
#include<iostream>
#include<algorithm>
using namespace std;

float distance(string a, string b) {
	int max_length=max(a.length(), b.length());
	transform(a.begin(),a.end(),a.begin(),::tolower);
	transform(b.begin(),b.end(),b.begin(),::tolower);
	
	int* costs = new int[b.length() + 1];
	for (int j = 0; j < b.length(); j++) {
		costs[j] = j;
	}
	for (int i = 1; i <= a.length(); i++) {
		// j == 0; nw = lev(i - 1, j)
		costs[0] = i;
		int nw = i - 1;
		for (int j = 1; j <= b.length(); j++) {
			int cj = min(1 + min(costs[j], costs[j - 1]),
					a[i - 1 ]== b[j - 1] ? nw : nw + 1);
			nw = costs[j];
			costs[j] = cj;
		}
	}
	return 1 - (float) costs[b.length()] / max_length;
}

//float normalizedDistance(string a, string b) {
//	return 1 - (float) distance(a, b) / (max(a.length(), b.length()));
//}

int main(){
	
	string a,b;
	cin>>a>>b;
	cout<<distance(a,b); 
	
	return 0;
}

