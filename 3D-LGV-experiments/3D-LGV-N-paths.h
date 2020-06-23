/*  *** struct Path ***
	struct Path: {source, weigh, {list of Edges}}
	generator: Path(x, y, z) - creates Path with source - (x, y, z)
	functions:
	addEdge(Edge e, Polynomial weight) - adds an edge to the current path
	addEdge(Edge e, int id) - same as prev. but weight = x_{id}
	print() - prints source, and seq. of edges
*/

/*  *** struct NPath *** 
	struct NPath: {permutation, weight, {list of Path}}
	generator: NPath(vector < int > permutation, vector < Path > paths) - calculates weight by 
																		 multiplying weights of each path in paths

	generator: NPath(vector < int > permutation) -  generates NPath with given permutation, paths are empty, weight = 1
	generator: NPath() - creates an empty Paths, but with weight = 1
	functions:
	print() - prints ;)
	addPath(int start, int finish, Path p) - sets perm[start] = finish, paths[start] = p
	getSign() - returns the sign of permutation

	vector < NPath > mult(vector <  NPath >, vector < Path >, int start, int finish) - does the same thing as polynomial multiplication
																		, but now with saving paths
*/

																		

struct Path
{
    int source_x, source_y, source_z;
    Polynomial weight;
    vector < Edge > edges;

    Path () 
    {
    	weight = Polynomial(1);
    }

    Path (int x, int y, int z): source_x(x), source_y(y), source_z(z), weight(1) {}

    void addEdge(Edge e, Polynomial w) // adds edge e with weight w
    {
        edges.pb(e);
        weight *= w;
    }

    void addEdge(Edge e, int id) // adds edge e with weight x_{id}
    {
        edges.pb(e);
        weight *= Xpower(id, 1);
    }

    void print()
    {
        cout << "--path print--" << endl;
        cout << "Source: " << source_x << ' ' << source_y << ' ' << source_z << endl;
        cout << "Edges: ";
        for (auto e : edges)
            cout << '(' << e.dx << ',' << e.dy << ',' << e.dz << ") ";
        cout << endl << "--------------" << endl;
    }
};

struct NPath 
{
	vector < int > perm;
	Polynomial weight;
	vector < Path > paths;

	NPath () 
	{		
		weight = Polynomial(1);
	};

	// generates NPath with given permutation, paths are empty, weight = 1
	NPath (vector < int > _perm): perm(_perm), weight(1) {}

	// calculates weight by multiplying weight of path
	NPath (vector < int > _perm, vector < Path > _paths) 
	{
		perm = _perm;
		paths = _paths;
		weight = Polynomial(1);
		for (auto it : paths)
			weight *= it.weight;
	}

	void addPath(int s, int f, Path p)
	{
		if ((int)perm.size() <= s)
			perm.resize(s + 1);
		if ((int)paths.size() <= s)
			paths.resize(s + 1);
		
		perm[s] = f;
		paths[s] = p;
		weight *= p.weight;		
	}

	int getSign()  // returns the sign on the permutation
	{	
		int res = 1;
		for (int i = 0; i < (int)perm.size(); i++)
			for (int j = 0; j < i; j++)
				if (perm[j] > perm[i])
					res *= -1;
		return res;
	}

	void print()
	{
		cout << "========N-Path print===================" << endl;
		cout << "Permutation" << endl << "A: ";
		for (int i = 0; i < (int)perm.size(); i++)
			cout << i << ' ';
		cout << endl << "B: ";	
		for (int i = 0; i < (int)perm.size(); i++)
			cout << perm[i] << ' ';
		cout << endl;
		for (auto it : paths)
			it.print();
		cout << "=======================================" << endl;	
	}
};






