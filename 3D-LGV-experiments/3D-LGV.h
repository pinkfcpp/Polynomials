static const int FLOOR = 0;
static const int WALL = 1;
static const int DOOR = 2;
static const int K = 10;

static const bool WONNASAVEPATH = true; // turn true if you want to use pathSavingLGV
const int INF = (int)1e9 + 228;

//I've put it here from experiments.h
int MAXP = 3; // max lam part
int MAX_X = 3; // number of x-vars (height)
int MAX_Z = 2; // number of z-vars (width)
int n = 2; // size of det


struct Edge
{
    int dx, dy, dz, type;
    bool isWeighted;
    int firstID;

    bool canUse(int x, int y, int z)
    {
        if (type == FLOOR && z == 0) {
            return false;
        }
        if (type == WALL && z != 0)
            return false;
        return true;
    }
};

#include "3D-LGV-N-paths.h"
#include "pathSavingLGV.h"

struct Paths
{
    int fx, fy, fz;
    vector < Edge > e;
    map < int, map < int, map < int, bool > > > was;
    Polynomial res;
    map < int, int > character;
    vector <Edge > path;

    int sx, sy, sz;
    vector < int > weights;
    
    vector < Path > vectorPath;

    Paths () {}

    Paths (int _fx, int _fy, int _fz, const vector < Edge > & _e): fx(_fx), fy(_fy), fz(_fz), e(_e) {}

    bool isInside(int x, int y, int z)
    {
        return abs(x) < K && abs(y) < K && abs(z) < K;
    }

    void dfs1(int x, int y, int z)
    {
        if (!isInside(x, y, z))
            return;
        if (was[x][y][z])
            return;
        was[x][y][z] = true;
        for (auto it : e)
        {
            int tx = x - it.dx;
            int ty = y - it.dy;
            int tz = z - it.dz;
            if (it.canUse(tx, ty, tz))
                dfs1(tx, ty, tz);
        }
    }

    void updateVectorPath()
    {
    	assert(WONNASAVEPATH);
    	assert((int)path.size() == (int)weights.size());

    	Path curPath(sx, sy, sz);
    	for (int i = 0; i < (int)path.size(); i++)
    	{
    		curPath.addEdge(path[i], weights[i]);
		}
		vectorPath.pb(curPath);
    }

    int tab = 0;
    void dfs2(int x, int y, int z)
    {
        if (!was[x][y][z])
            return;
        // for (int i = 0; i < tab; i++) cerr << "  ";
        // for (auto [id, deg] : character) cerr << id << ' ' << deg << ", "; cerr << "\n";
        if (x == fx && y == fy && z == fz)
        {
            // for (int i = 0; i < tab; i++) cerr << "  ";
            // cerr << "added: ";
            // for (auto [id, deg] : character) cerr << id << ' ' << deg << ", "; cerr << "\n";
            res.add(character, 1);

            if (WONNASAVEPATH)
            	updateVectorPath();
            return;
        }
        for (auto it : e)
        {
            // for (int i = 0; i < tab; i++) cerr << "  ";
            // cerr << "try: " << it.firstID << "\n";
            if (!it.canUse(x, y, z))
                continue;
            if (it.isWeighted)
            {
                int id;
                if (it.type == FLOOR) {
                    id = it.firstID - z;
                }
                else if (it.type == WALL)
                    id = it.firstID + MAX_X + 1 - y;
                else if (it.type == DOOR)
                    id = it.firstID + x;
                character[id]++;
                
                if (WONNASAVEPATH)
                	weights.pb(id);
                
                path.pb(it);
                tab++;
                dfs2(x + it.dx, y + it.dy, z + it.dz);
                tab--;
                character[id]--;
                if (character[id] == 0)
                    character.erase(id);
                path.pop_back();

                if (WONNASAVEPATH)
                	weights.pop_back();
            }
            else
            {
            	if (WONNASAVEPATH)
            		weights.pb(INF);
                
                path.pb(it);
                tab++;
                dfs2(x + it.dx, y + it.dy, z + it.dz);
                tab--;
                path.pop_back();
                
                if (WONNASAVEPATH)
                	weights.pop_back();
            }
        }
    }
};

Polynomial W(int x1, int y1, int z1, int x2, int y2, int z2, const vector < Edge > & e)
{
    Paths P(x2, y2, z2, e);
    P.dfs1(x2, y2, z2);
    P.dfs2(x1, y1, z1);
    P.res.normalize();
    return P.res;
}

Polynomial Wmodified(int x1, int y1, int z1, int x2, int y2, int z2, const vector < Edge > & e, vector < Path > &vp)
{
	assert(WONNASAVEPATH);    

    Paths P(x2, y2, z2, e);
    P.sx = x1, P.sy = y1, P.sz = z1;

    P.dfs1(x2, y2, z2);
    P.dfs2(x1, y1, z1);
    P.res.normalize();
    vp = P.vectorPath;
    return P.res;
}

struct TreeDimLGV
{
    vector < Edge > e;
    vector < int > Ax, Ay, Az;
    vector < int > Bx, By, Bz;
    vector < vector < Polynomial > > A;
	PathMatrix D;

    void addEdge(int dx, int dy, int dz, int type, bool isWeighted, int firstID)
    {
        e.pb({dx, dy, dz, type, isWeighted, firstID});
    }

    void addSource(int x, int y, int z)
    {
        Ax.pb(x);
        Ay.pb(y);
        Az.pb(z);
    }

    void addSink(int x, int y, int z)
    {
        Bx.pb(x);
        By.pb(y);
        Bz.pb(z);
    }
    void calcLGV()
    {
        int n = (int)Ax.size();
        A = vector < vector < Polynomial > >(n, vector < Polynomial > (n));
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                A[i][j] = W(Ax[i], Ay[i], Az[i], Bx[j], By[j], Bz[j], e);
    }
    bool isInteresting() {
        int n = (int)A.size();
        vector < int > p;
        for (int i = 0; i < n; i++)
            p.pb(i);
        do
        {
            bool is_alive = true;
            for (int i = 0; i < n; i++)
                is_alive &= A[i][p[i]].p.size() > 0;

            if (is_alive && sign(p) < 0)
                return true;
        }
        while(next_permutation(p.begin(), p.end()));
        return false;
    }
    Polynomial getLGV() {
        return det(A);
    }
    Polynomial getTrace() {
        Polynomial res(1);
        for (int i = 0; i < (int)A.size(); i++)
            res *= A[i][i];
        return res;
    }

    Polynomial LGVmodified()
    {
    	assert(WONNASAVEPATH);
    	int n = (int)Ax.size();
    	D = PathMatrix(n, PathRow(n));
        vector < vector < Polynomial > > A(n, vector < Polynomial > (n));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                A[i][j] = Wmodified(Ax[i], Ay[i], Az[i], Bx[j], By[j], Bz[j], e, D[i][j]);
            }
        }
        /*cout << "!!!" << endl;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                cout << A[i][j].p.size() << ' ';
            cout << endl;
        }*/
        for (int i = 0; i < n; i++, cout << endl)
		{
			for (int j = 0; j < n; j++, cout << endl)
			{
				cout << i << " --> " << j << endl;
				for (auto it : D[i][j])
				{
					it.print();
					it.getWeight().print();		
				}
			}
		}        
        return det(A);
    }

    Polynomial LGV()
    {
    	if (WONNASAVEPATH)
    	{
    		return LGVmodified();
        }
        int n = (int)Ax.size();
        vector < vector < Polynomial > > A(n, vector < Polynomial > (n));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                A[i][j] = W(Ax[i], Ay[i], Az[i], Bx[j], By[j], Bz[j], e);
            }
        }
        cout << "!!!" << endl;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                cout << A[i][j].p.size() << ' ';
            cout << endl;
        }
        return det(A);
    }

    void print(bool full = false)
    {
        int n = (int)Ax.size();
        cout << "Sources: " << endl;
        for (int i = 0; i < n; i++)
            cout << Ax[i] << ' '<< Ay[i] << ' ' << Az[i] << endl;
        cout << "Sinks: " << endl;
        for (int i = 0; i < n; i++)
            cout << Bx[i] << ' ' << By[i] << ' ' << Bz[i] << endl;
        cout << "Matrix:\n";
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                cout << A[i][j].p.size() << ' ';
            cout << "\n";
        }
        if (full) {
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++) {
                    cout << "A[" << i + 1 << ' ' << j + 1 << "] = ";
                    A[i][j].print();
                }
        }
    }

    void GrothendieckInit(string lambda, string mu, int M)
    {
        int n = lambda[0] - '0';
        for (int i = 0; i < n; i++)
        {
            int _l = 0;
            for (int x = 0; x < (int)lambda.size(); x++)
                _l += bool(lambda[x] - '0' >= i + 1);
            addSource(i, 0, _l - 1);
        }
        for (int i = 0; i < n; i++)
        {
            int _m = 0;
            for (int x = 0; x < (int)mu.size(); x++)
                _m += bool(mu[x] - '0' >= i + 1);
            addSink(i + 1, M, _m);
        }
        addEdge(0, 1, 0, WALL, false, M);
        addEdge(1, 1, 0, WALL, true, M);
        addEdge(0, 0, 1, FLOOR, true, M + 1);
        addEdge(-1, 0, 1, FLOOR, false, M + 1);
    }

    void checkItOut()
    {
        int n = (int)Ax.size();
        vector < vector < Polynomial > > A(n, vector < Polynomial > (n));
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                A[i][j] = W(Ax[i], Ay[i], Az[i], Bx[j], By[j], Bz[j], e);

        cout << "Matrix:" << endl;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                cout << A[i][j].p.size() << ' ';
            cout << endl;
        }

        vector < int > sets;
        for (int mask = 1; mask < (1 << n); mask++)
            if (__builtin_popcount(mask) <= 4)
                sets.pb(mask);

        int numberOfPairs = 0, currentPair = 0;
        for (auto mask1 : sets)
            for (auto mask2 : sets)
                numberOfPairs += bool(__builtin_popcount(mask1) == __builtin_popcount(mask2));

        for (auto mask1 : sets)
        {
            for (auto mask2 : sets)
            {
                int m = __builtin_popcount(mask1);
                if (m != __builtin_popcount(mask2))
                    continue;

                currentPair++;

                vector < int > a, b;
                for (int i = 0; i < n; i++)
                    if (mask1 & (1 << i))
                        a.pb(i);
                for (int i = 0; i < n; i++)
                    if (mask2 & (1 << i))
                        b.pb(i);

                vector < vector < Polynomial > > B(m, vector < Polynomial > (m));
                for (int i = 0; i < m; i++)
                    for (int j = 0; j < m; j++)
                        B[i][j] = A[a[i]][b[j]];

                Polynomial cur = det(B);
                if (cur.p.empty())
                    cout << "Empty" << endl;
                else if (cur.isPositive())
                    cout << "Positive!" << endl;
                else
                {
                        cout << "Negative!" << endl;
                        cout << "Edges: " << endl;
                        for (auto it : e)
                        {
                            cout << it.dx << ' ' << it.dy << ' ' << it.dz << ' ';
                            if (it.type == WALL)
                                cout << "WALL" << endl;
                            else
                                cout << "FLOOR" << endl;
                        }
                        cout << endl;
                        cout << "Sources: " << endl;
                        for (auto id : a)
                            cout << Ax[id] << ' ' << Ay[id] << ' ' << Az[id] << endl;
                        cout << endl << "Sinks: " << endl;
                        for (auto id : b)
                            cout << Bx[id] << ' ' << By[id] << ' ' << Bz[id] << endl;
                        cout << endl;
                        return;
                }
                cout << "(" << currentPair << "/" << numberOfPairs << ")" << endl;
            }
        }
        cout << "Verdict = All positive!!!" << endl;
    }
    void randomInit(int n)
    {
        int lastX = 0, lastZ = 0;
        for (int i = 0; i < n; i++)
        {
            lastX += rand() % 3 + 1;
            lastZ += rand() % 3;
            Ax.pb(lastX);
            Ay.pb(0);
            Az.pb(lastZ);
        }
        //lastX = 0;
        //lastZ = 0;
        int M = 5;
        for (int i = 0; i < n; i++)
        {
            lastX += rand() % 3 + 1;
            lastZ += rand() % 3;
            Bx.pb(lastX);
            By.pb(M);
            Bz.pb(lastZ);
        }
        for (int i = 0; i < n; i++)
            cout << Ax[i] << ' ' << Ay[i] << ' ' << Az[i] << endl;
        cout << endl;
        for (int i = 0; i < n; i++)
            cout << Bx[i] << ' ' << By[i] << ' ' << Bz[i] << endl;
    }
};

void Check(vector < int > Ax, int y, vector < int > Az, TreeDimLGV &graph)
{
    vector < int > xmasks, zmasks;
    for (int mask = 1; mask < (1 << (int)Ax.size()); mask++)
        if (__builtin_popcount(mask) <= 4)
            xmasks.pb(mask);
    for (int mask = 1; mask < (1 << (int)Az.size()); mask++)
        if (__builtin_popcount(mask) <= 4)
            zmasks.pb(mask);
    vector < pair < pair < int, int >, pair < int, int > > > test;
    for (auto xmask1 : xmasks)
    {
        for (auto xmask2 : xmasks)
        {
            if (__builtin_popcount(xmask1) != __builtin_popcount(xmask2))
                continue;
            for (auto mask1 : zmasks)
            {
                if (__builtin_popcount(xmask1) != __builtin_popcount(mask1))
                    continue;
                for (auto mask2 : zmasks)
                {
                    if (__builtin_popcount(mask2) != __builtin_popcount(mask1))
                        continue;
                    test.pb({{xmask1, xmask2}, {mask1, mask2}});
                }
            }
        }
    }
    for (int id = 0; id < (int)test.size(); id++)
    {
        auto it = test[id];
        vector < int > x1, x2, z1, z2;
        for (int i = 0; i < (int)Ax.size(); i++)
            if (it.first.first & (1 << i))
                x1.pb(Ax[i]);
        for (int i = 0; i < (int)Ax.size(); i++)
            if (it.first.second & (1 << i))
                x2.pb(Ax[i]);
        for (int i = 0; i < (int)Az.size(); i++)
            if (it.second.first & (1 << i))
                z1.pb(Az[i]);
        for (int i = 0; i < (int)Az.size(); i++)
            if (it.second.second & (1 << i))
                z2.pb(Az[i]);

        TreeDimLGV Lattice = graph;
        for (int i = 0; i < (int)x1.size(); i++)
        {
            Lattice.addSource(x1[i], 0, z1[i]);
            Lattice.addSink(x2[i], y, z2[i]);
        }
        cout << '(' << id + 1 << '/' << (int)test.size() << ')' << endl;
        Polynomial res = Lattice.LGV();
        if (res.p.empty())
            cout << "Empty!" << endl;
        else if (res.isPositive())
            cout << "Positivity!" << endl;
        else
        {
            cout << "Negative!" << endl;
            for (int i = 0; i < (int)x1.size(); i++)
                cout << x1[i] << ' ' << 0 << ' ' << z1[i] << endl;
            cout << endl;
            for (int i = 0; i < (int)x1.size(); i++)
                cout << x2[i] << ' ' << y << ' ' << z2[i] << endl;
            return;
        }
    }
    cout << "Verdict = Positive!!!" << endl;
}

void straightCheck(vector < int > Ax, int y, vector < int > Az, TreeDimLGV graph)
{
    vector < int > xmasks, zmasks;
    for (int mask = 1; mask < (1 << (int)Ax.size()); mask++)
        if (__builtin_popcount(mask) <= 4)
            xmasks.pb(mask);
    for (int mask = 1; mask < (1 << (int)Az.size()); mask++)
        if (__builtin_popcount(mask) <= 4)
            zmasks.pb(mask);
    vector < pair < int, pair < int, int > > > test;
    for (auto xmask1 : xmasks)
    {
        for (auto mask1 : zmasks)
        {
            if (__builtin_popcount(xmask1) != __builtin_popcount(mask1))
                continue;
            for (auto mask2 : zmasks)
            {
                if (__builtin_popcount(mask2) != __builtin_popcount(mask1))
                    continue;
                test.pb({xmask1, {mask1, mask2}});
            }
        }
    }
    for (int id = 0; id < (int)test.size(); id++)
    {
        auto it = test[id];
        vector < int > xs, z1, z2;
        for (int i = 0; i < (int)Ax.size(); i++)
            if (it.first& (1 << i))
                xs.pb(Ax[i]);
        for (int i = 0; i < (int)Az.size(); i++)
            if (it.second.first & (1 << i))
                z1.pb(Az[i]);
        for (int i = 0; i < (int)Az.size(); i++)
            if (it.second.second & (1 << i))
                z2.pb(Az[i]);
        TreeDimLGV Lattice = graph;
        for (int i = 0; i < (int)xs.size(); i++)
        {
            Lattice.addSource(xs[i], 0, z1[i]);
            Lattice.addSink(xs[i], y, z2[i]);
        }
        cout << '(' << id + 1 << '/' << (int)test.size() << ')' << endl;
        Polynomial res = Lattice.LGV();
        if (res.p.empty())
            cout << "Empty!" << endl;
        else if (res.isPositive())
            cout << "Positivity!" << endl;
        else
        {
            cout << "Negative!" << endl;
            for (int i = 0; i < (int)xs.size(); i++)
                cout << xs[i] << ' ' << 0 << ' ' << z1[i] << endl;
            cout << endl;
            for (int i = 0; i < (int)xs.size(); i++)
                cout << xs[i] << ' ' << y << ' ' << z2[i] << endl;
            return;
        }
    }
    cout << "Verdict = Positive!!!" << endl;
}


void generate_layer_lattice_xy_weakly_increasing(TreeDimLGV & lattice, int n)
{
    int z = rand() % 4 + 1;
    int x = 0, y = 0;
    for (int i = 0; i < n; i++)
    {
        x += rand() % 3;
        y += rand() % 3;
        lattice.addSource(x, y, 0);
    }
    x = 0, y = 0;
    for (int i = 0; i < n; i++)
    {
        x += rand() % 3;
        y += rand() % 3;
        lattice.addSink(x, y, z);
    }
}
