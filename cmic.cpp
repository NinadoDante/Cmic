
#include "cmic.h"
#include "libzpaq.h"
#include <pthread.h>
#include <thread>
#include <algorithm>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cstdio>
#include <cstdlib>
#include <malloc.h>

using namespace libzpaq;
using namespace std;

#define MAXTABLESIZE 10000 
namespace cmic {

typedef void* ThreadReturn;                               
void run(ThreadID& tid, ThreadReturn(*f)(void*), void* arg)
    {pthread_create(&tid, NULL, f, arg);}
void join(ThreadID tid) {pthread_join(tid, NULL);}        
typedef pthread_mutex_t Mutex;                             
void init_mutex(Mutex& m) {pthread_mutex_init(&m, 0);}     
void lock(Mutex& m) {pthread_mutex_lock(&m);}             
void release(Mutex& m) {pthread_mutex_unlock(&m);}        
void destroy_mutex(Mutex& m) {pthread_mutex_destroy(&m);}  

typedef int ElementType;
struct LNode
{
	ElementType data;
	LNode *next;
};
typedef LNode *PtrToNode;
typedef PtrToNode LinkList;
struct TblNode
{
	int tablesize;  
	LinkList heads; 
};
typedef struct TblNode *HashTable;

int NextPrime(int n)
{
	int p = (n % 2) ? n + 2 : n + 1;
	int i;
	while (p <= MAXTABLESIZE)
	{
		for (i = (int)sqrt(p); i > 2; i--)
		{
			if ((p % i) == 0)
				break;
		}
		if (i == 2)
			break; 
		else
			p += 2;
	}
	return p;
}


HashTable CreateTable(int table_size)
{
	HashTable h = (HashTable)malloc(sizeof(TblNode));
	h->tablesize = NextPrime(table_size);
	h->heads = (LinkList)malloc(h->tablesize * sizeof(LNode));
	for (int i = 0; i < h->tablesize; i++)
	{
		h->heads[i].next = NULL;
	}
	return h;
}

int Hash(ElementType key, int n)
{
	return key % 11;
}

LinkList Find(HashTable h, ElementType key1, ElementType key2)
{
	int pos1;
	int pos2;
	pos1 = Hash(key1, h->tablesize);
	pos2 = Hash(key2, h->tablesize);
	LinkList p = h->heads[pos1].next;
	uint32_t l = 0xffffffffu, r = 0;
	for (int m = 0; m < h->tablesize; m++) {
		LinkList p = h->heads[m].next;
		while (p)
		{
			if (i == b.bucket && p->data > L && p->data < R) {
				in[j] = true;
				l = min(l, b.eline);
				r = max(r, b.eline);
			}
			p = p->next;
		}
	}
	auto &_val = job->val[i];
	job->mx[i] = lower_bound(_val.begin(), _val.end(), L + 1) - _val.begin() - 1;
	job->flag[i][job->mx[i]] = 2;
	L = l;
	l = r; r = fmt.qlen[(i + 1) % 2];
	for (auto& b : job->binfo) if (i == b.bucket) {
		if (b.eline > l && b.eline < r) r = b.eline;
	}
	R = r;
	return R;
}


bool Insert(HashTable h, ElementType key)
{
	LinkList p = Find(h, key); 
	if (!p)
	{
		LinkList new_cell = (LinkList)malloc(sizeof(LNode));
		new_cell->data = key;
		int pos = Hash(key, h->tablesize);
		new_cell->next = h->heads[pos].next;
		h->heads[pos].next = new_cell;
		return true;
	}
	else
	{
		cout << "This address already exists " << endl;
		return false;
	}
}

/*Destroy this table*/
void DestroyTable(HashTable h)
{
	int i;
	LinkList p, tmp;
	//释放每个节点
	for (i = 0; i < h->tablesize; i++)
	{
		p = h->heads[i].next;
		while (p)
		{
			tmp = p->next;
			free(p);
			p = tmp;
		}
	}
	free(h->heads);
	free(h);
}
class Semaphore {
public:
    Semaphore() {sem=-1;}
    void init(int n) {
        assert(n>=0);
        assert(sem==-1);
        pthread_cond_init(&cv, 0);
        pthread_mutex_init(&mutex, 0);
        sem=n;
    }
	void signal() {
		assert(sem >= 0);
		pthread_mutex_lock(&mutex);
		++sem;
		pthread_cond_signal(&cv);
		pthread_mutex_unlock(&mutex);
	}
    int wait() {
        assert(sem>=0);
        pthread_mutex_lock(&mutex);
        int r=0;
        if (sem==0) r=pthread_cond_wait(&cv, &mutex);
        assert(sem>0);
        --sem;
        pthread_mutex_unlock(&mutex);
        return r;
    }
	void destroy() {
		assert(sem >= 0);
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cv);
	}
private:
    pthread_cond_t cv;  // to signal FINISHED
    pthread_mutex_t mutex; // protects cv
    int sem;  // semaphore count
};

struct QuaBlock {
    enum Tag {QUALITY} tag;
    int64_t pos;
	uint32_t start;
	uint32_t end;
	uint32_t length;
    uint32_t eline;
    uint8_t bucket;

    void write(FILE* fw) {
        char _tag = tag;
        fwrite(&_tag, sizeof _tag, 1, fw);
        fwrite(&pos, sizeof pos, 1, fw);
        fwrite(&length, sizeof length, 1, fw);
        fwrite(&start, sizeof start, 1, fw);
        fwrite(&end, sizeof end, 1, fw);
        fwrite(&eline, sizeof eline, 1, fw);
        fwrite(&bucket, sizeof bucket, 1, fw);
    }

    bool read(FILE* fp) {
        char _tag;
        if(fread(&_tag, sizeof _tag, 1, fp) == 0) return false;
        tag = Tag(_tag);
        fread(&pos, sizeof pos, 1, fp);
        fread(&length, sizeof length, 1, fp);
        fread(&start, sizeof start, 1, fp);
        fread(&end, sizeof end, 1, fp);
        fread(&eline, sizeof eline, 1, fp);
        fread(&bucket, sizeof bucket, 1, fp);
        return true;
    }
};
struct IndexInfo: public QuaBlock {
	enum Tag { QUALITY } tag;
	int64_t pos;
	uint32_t length;
	uint32_t start;
	uint32_t eline;
	uint8_t bucket;
	int64_t n;
	void write(FILE* fw) {}
	bool read(FILE* fp) {}
};
struct CJ {
	QuaBlock info;
	IndexInfo info2;
    enum {EMPTY, FULL, COMPRESSING, COMPRESSED, WRITING} state;
    StringBuffer in;       // uncompressed input
    StringBuffer out;      // compressed output
    string comment;        // if "" use default
    string method;         // compression level or "" to mark end of data
    Semaphore full;        // 1 if in is FULL of data ready to compress
    CJ(): state(EMPTY) {}
};

struct FileWriter: public libzpaq::Writer {
    FILE* fp;
    FileWriter(const char* filename) {
        fp = fopen(filename, "wb");
    }

    ~FileWriter() {
        if(fp) {
            fclose(fp);
            fp = NULL;
        }
    }

    void seek(int offset) {
        fseek(fp, offset, SEEK_SET);
    }

    void put(int c) {
        putc(c, fp);
    }

    void write(const char* buf, int n) {
        fwrite(buf, 1, n, fp);
    }

    int64_t tell() {
        return ftello(fp);
    }
};

// Instructions to a compression job
class CompressJob {
public:
    Mutex mutex;           
    FileWriter* out;
    char score;
private:
    int job;              
    CJ* q;                 
    unsigned qsize;       
    int front;            
    Semaphore empty;       
    Semaphore compressors; 
    Semaphore compressed;
    queue<int> wq;
    bool finished;
    int count;
public:
    friend ThreadReturn compressThread(void* arg);
    friend ThreadReturn writeThread(void* arg);
    CompressJob(int threads, int buffers): job(0), q(0), qsize(buffers), front(0), finished(false), count(0) {
        q=new CJ[buffers];
        if (!q) throw std::bad_alloc();
        init_mutex(mutex);
        empty.init(buffers);
        compressors.init(threads);
        compressed.init(0);
        for (int i=0; i<buffers; ++i) {
            q[i].full.init(0);
        }
    }
    ~CompressJob() {
        for (int i=qsize-1; i>=0; --i) {
            q[i].full.destroy();
        }
        compressed.destroy();
        compressors.destroy();
        empty.destroy();
        destroy_mutex(mutex);
        delete[] q;
    }      
    void write(StringBuffer& s, QuaBlock::Tag _tag, uint8_t bucket, uint32_t start, uint32_t end, uint32_t eline, string method);
	void Index (StringBuffer& s, IndexInfo::Tag _tag, uint8_t bucket, uint32_t start, uint32_t eline, string method);
    vector<QuaBlock> binfo;
    void write_binfo() {
        for(auto& b : binfo) {
            b.write(out->fp);
        }
    }
};

void CompressJob::write(StringBuffer& s, QuaBlock::Tag _tag, uint8_t bucket, uint32_t start, uint32_t end, uint32_t eline, string method) {
    for (unsigned k=(method=="")?qsize:1; k>0; --k) {
        empty.wait();
        lock(mutex);
        ++count;
        unsigned i, j;
        for (i=0; i<qsize; ++i) {
            if (q[j=(i+front)%qsize].state==CJ::EMPTY) {
                q[j].info.tag=_tag;
                q[j].info.bucket=bucket;
                q[j].info.start=start;
                q[j].info.end=end;
                q[j].info.eline=eline;
                q[j].comment="";
                q[j].method=method;
                q[j].in.resize(0);
                q[j].in.swap(s);
                q[j].state=CJ::FULL;
                q[j].full.signal();
                break;
            }
        }
        release(mutex);
        assert(i<qsize);  
    }
}
void CompressJob::Index (StringBuffer& s, IndexInfo::Tag _tag, uint8_t bucket, uint32_t start, uint32_t eline, string method) {
	for (unsigned k = (method == "") ? qsize : 1; k>0; --k) {
		empty.wait();
		lock(mutex);
		++count;
		unsigned i, j;
		for (i = 0; i<qsize; ++i) {
			if (q[j = (i + front) % qsize].state == CJ::EMPTY) {
				q[j].info2.tag = _tag;
				q[j].info2.bucket = bucket;
				q[j].info2.start = start;
				q[j].info2.eline = eline;
				q[j].comment = "";
				q[j].method = method;
				q[j].in.resize(0);
				q[j].in.swap(s);
				q[j].state = CJ::FULL;
				q[j].full.signal();
				if (score == "~") {
					q[j].info2.n++;
				}
				q[j].info2.end = q[j].info2.start + count + q[j].info2.n;
				break;
			}
		}
		release(mutex);
		assert(i<qsize);
	}
}

void mapping(StringBuffer& in, char score) {
	StringBuffer out;
	out.resize(in.size() / 2);
	out.resize(0);
	int len = 0; // pending output bytes
	int len2 = 0;
	int j = 0, k = 0, l2 = max(33, score - 7), l3 = l2 + 4, r = l2 + 7; // last 2 bytes
	int m = 0, n = 0, l4 = score + 7, l5 = l4 - 4, o = l4 - 7;
	for (int c = 0; (c = in.get()) != EOF; k = j, j = c, n = m, m = o) {
		if (c <= r) {
			if (len == 0 && (c == score || c >= l2 && c <= r)) ++len;
			else if (len == 1 && (c == score && j == score || c >= l3 && c <= r && j >= l3 && j <= r)) ++len;
			else if (len >= 2 && len<55 && k == score && j == score && c == score) ++len;
			else {  // must write pending output
				++len;  // c is pending
				if (len>2 && j == score && k == score || len == 2 && j == score)
					out.put(199 + len), len = 1; // [201, 255]
				if (len == 3) {
					if (c >= l3 && c <= r)
						out.put(137 + (c - l3) + 4 * (j - l3) + 16 * (k - l3)), len = 0; // [137, 200]
					else
						out.put(73 + (c - l2) + 8 * (j - l2)), len = 1; // [109, 112], [117, 120], [125, 128], [133, 136]
				}
				if (len == 2) {
					if (c >= l2 && c <= r) out.put(73 + (c - l2) + 8 * (j - l2)), len = 0; // [73, 108], [113, 116], [121, 124], [129, 132]
					else out.put(j - 32), len = 1; // [32, 39]
				}
				if (len == 1) {
					if (c == 10) {
						len = j = k = 0;
						out.put(0);
						continue;
					}
					if (c<l2 || c>r) out.put(c - 32), len = 0; // [4, 31], [40, 72]
				}
			}
		}
		if (c > r) {
			if (len2 == 0 && (c == score || c <= l4 && c >= o)) ++len2;
			else if (len2 == 1 && (c == score && n == score || c <= l5 && c >= o && n <= l5 && n >= o)) ++len2;
			else if (len2 >= 2 && len2 < 55 && n == score && m == score && c == score) ++len2;
			else {  // must write pending output
				++len2;  // c is pending
				if (len2 > 2 && m == score && n == score || len2 == 2 && m == score)
					out.put(199 + len2), len2 = 1; // [201, 255]
				if (len2 == 3) {
					if (c <= l5 && c >= o)
						out.put(319 + (l5 - c) + 4 * (l5 - m) + 16 * (l5 - n)), len2 = 0; // [319, 382]
					else
						out.put(255 + (l4 - m) + 8 * (l4 - c)), len2 = 1; // [219, 294], [299, 302], [307, 310], [315, 318]
				}
				if (len2 == 2) {
					if (c <= l4 && c >= o) out.put(255 + (l4 - c) + 8 * (l4 - n)), len2 = 0; // [255, 290], [295, 298], [303, 306], [311, 314]
					else out.put(n - 32), len2 = 1; // [32, 39]
				}
				if (len2 == 1) {
					if (c == 10) {
						len2 = n = m = 0;
						out.put(0);
						continue;
					}
					if (c<o || c>l4) out.put(c - 32), len2 = 0; // [4, 31], [40, 72]
				}
			}
		}
	}
	out.swap(in);
}

// Compress data in the background, one per buffer
ThreadReturn compressThread(void* arg) {
    CompressJob& job=*(CompressJob*)arg;
    int jobNumber=0;
    try {

        // Get job number = assigned position in queue
        lock(job.mutex);
        jobNumber=job.job++;
        assert(jobNumber>=0 && jobNumber<int(job.qsize));
        CJ& cj=job.q[jobNumber];
        release(job.mutex);

        // Work until done
        while (true) {
            cj.full.wait();
            lock(job.mutex);

            // Check for end of input
            if (cj.method=="") {
                job.wq.push(jobNumber);
                job.compressed.signal();
                release(job.mutex);
                return 0;
            }

            // Compress
            assert(cj.state==CJ::FULL);
            cj.state=CJ::COMPRESSING;
            release(job.mutex);
            job.compressors.wait();
            if(cj.info.tag == QuaBlock::QUALITY) mapping(cj.in, job.score);
            libzpaq::compressBlock(&cj.in, &cj.out, cj.method.c_str(), "", cj.comment.c_str(), false);
            cj.in.resize(0);
            lock(job.mutex);
            cj.state=CJ::COMPRESSED;
            job.wq.push(jobNumber);
            job.compressed.signal();
            job.compressors.signal();
            release(job.mutex);
        }
    }
    catch (std::exception& e) {
        lock(job.mutex);
        fflush(stdout);
        fprintf(stderr, "job %d: %s\n", jobNumber+1, e.what());
        release(job.mutex);
        exit(1);
    }
    return 0;
}

void compressor::init(param _par) {
    par = _par;
    job->out = new FileWriter(par.out_name);
    job->out->seek(17);
}

// Write compressed data in the background
ThreadReturn writeThread(void* arg) {
    CompressJob& job=*(CompressJob*)arg;
    try {

        // work until done
        while (true) {
            if(job.finished && job.count == 0) return 0;

            // wait for something to write
            job.compressed.wait();
            lock(job.mutex);
            CJ& cj=job.q[job.wq.front()];
            job.wq.pop();
            --job.count;

            // Quit if end of input
            if (cj.method=="") {
                job.finished = true;
                release(job.mutex);
                continue;
            }

            // Write
            assert(cj.state==CJ::COMPRESSED);
            cj.state=CJ::WRITING;
            if (job.out && cj.out.size()>0) {
                release(job.mutex);
                assert(cj.out.c_str());
                const char* p=cj.out.c_str();
                uint32_t n=cj.out.size();
                const uint32_t N=1<<30;
                cj.info.pos=job.out->tell();
                while (n>N) {
                    job.out->write(p, N);
                    p+=N;
                    n-=N;
                }
                job.out->write(p, n);
                cj.info.length=job.out->tell() - cj.info.pos;
                lock(job.mutex);
            }
            cj.out.resize(0);
            cj.state=CJ::EMPTY;
            job.front=(job.front+1)%job.qsize;
            job.binfo.push_back(cj.info);
            job.empty.signal();
            release(job.mutex);
        }
    }
    catch (std::exception& e) {
        fflush(stdout);
        fprintf(stderr, "zpaq exiting from writeThread: %s\n", e.what());
        exit(1);
    }
    return 0;
}

const size_t BUFFER_SIZE = 1 << 50;
const char METHOD[] = "55,220,0";

compressor::compressor(int _threads): threads(_threads) {
    if(threads < 1) threads = thread::hardware_concurrency();
    tid.resize(threads*2-1);
    job = new CompressJob(threads, tid.size());
    for (unsigned i=0; i<tid.size(); ++i) run(tid[i], compressThread, job);
    run(wid, writeThread, job);
}

void compressor::end() {
    StringBuffer _;
    job->write(_, QuaBlock::Tag(0), 0, 0, 0, 0, "");  // signal end of input
    for (unsigned i=0; i<tid.size(); ++i) join(tid[i]);
    join(wid);
    int64_t len = job->out->tell();
    job->write_binfo();
    job->out->seek(0);
    fmt.write(job->out->fp);
    fwrite(&len, sizeof len, 1, job->out->fp);
    delete job->out;
    delete job;
}

void compressor::get_score(const vector<string>& sample) {
    int score_cnt[128];
    memset(score_cnt, 0, sizeof score_cnt);
    for(auto& s : sample) {
        for(char c : s) ++score_cnt[c];
    }
    fmt.score = max_element(score_cnt, score_cnt + 128) - score_cnt;
}

double compressor::get_table(const vector<string>& sample, unordered_map<long long, double>& table, int k) {
    unordered_map<long long, int> mp;
    int tot = 0;
    for(auto& s : sample) {
        for(size_t i = k-1; i < s.size(); ++i) {
            long long val = 0;
            for(size_t l = i+1-k; l <= i; ++l) val = val << 7 | s[l];
            ++mp[val];
		}
		tot += s.size()+1-k;
    }
    vector<pair<int, long long>> vec;
    for(auto& _ : mp) vec.emplace_back(_.second, _.first);
    sort(vec.begin(), vec.end(), greater<pair<int, long long>>());
    int cnt = tot * 0.7;
    for(auto it = vec.begin(); cnt > 0; ++it) {
        table[it->second] = it->first / (double)tot;
        cnt -= it->first;
    }
    double mx = 0;
    for(auto& s : sample) {
        double score = 0;
        for(size_t i = k-1; i < s.size(); ++i) {
            long long val = 0;
            for(size_t l = i+1-k; l <= i; ++l) val = val << 7 | s[l];
            score += table[val];
        }
        mx = max(mx, score/(s.size()+1-k));
    }
    return mx;
}

char _s[51234];

void compressor::qs_compress() {
    double border;
    const double threshold = par.threshold;
    const int k = par.k;
    unordered_map<long long, double> table;
    table.max_load_factor(0.5);
    uint32_t num = 0;
    string s;
    vector<string> sample(100000, s);
    {
        while(num < 100000 && gets(_s)) sample[num++] = _s;
        sample.resize(num);
        get_score(sample);
        border = get_table(sample, table, k) * par.threshold;
        job->score = fmt.score;
    }
    StringBuffer sb[2];
    uint32_t cur[3]{}, pre[2]{}, eline[2]{};
    long long base = 1;
    for(int i = 1; i < k; ++i) base = base << 7;
    base -= 1;
    int flag = 0;
    num = 0;
    while(true) {
        if(num < sample.size()) s.swap(sample[num++]);
        else if(gets(_s)) s = _s;
        else break;
        long long val = 0;
        for(int i = 0; i < k-1; ++i) val = val << 7 | s[i];
        double score = 0;
        for(size_t j = k-1; j < s.size(); ++j) {
            val = (val & base) << 7 | s[j];
            auto it = table.find(val);
            if(it != table.end()) score += it->second;
        }
        size_t res = score < border*(s.size()+1-k);
        StringBuffer& temp = sb[res];
        ++cur[res];
        temp.write(s.c_str(), s.size());
        temp.put('\n');
        if(res > 0) {
            sb[0].put('\n');
            ++cur[0];
        }
        if(temp.size() > BUFFER_SIZE) {
            job->write(temp, QuaBlock::QUALITY, res, pre[res], cur[res], eline[res], METHOD);
            pre[res] = cur[res];
            eline[res] = cur[res+1];
            if(res) ++flag;
            else flag = 0;
        }
        if(flag >= 10) {
            job->write(sb[0], QuaBlock::QUALITY, 0, pre[0], cur[0], eline[0], METHOD);
            pre[0] = cur[0];
            eline[0] = cur[1];
            flag = 0;
        }
    }
    for(size_t i = 0; i < 2; ++i) {
        if(sb[i].size() > 0) {
            job->write(sb[i], QuaBlock::QUALITY, i, pre[i], cur[i], eline[i], METHOD);
        }
    }
    for(int i = 0; i < 2; ++i) fmt.qlen[i] = cur[i];
}
 
struct Block {
    StringBuffer* in;
    enum {READY, WORKING, GOOD, BAD} state;
    int id, info;
    Block(int _id = -1, int _info = 0): state(READY), id(_id), info(_info), in(new StringBuffer) {}

    void operator = (const Block& b) {
        in = b.in; state = b.state; id = b.id;
    }
};

struct ExtractJob {         // list of jobs
    Mutex mutex;              // protects state
    Mutex read_mutex;
    int job;                  // number of jobs started
    FILE *fp, *fout;
    uint32_t L, R;
    vector<Block> block;      // list of data blocks to extract
    vector<QuaBlock> binfo;
    vector<string>* qout;
    vector<int8_t> flag[2];
    vector<uint32_t> val[2];
    uint32_t cur[2];
    uint32_t mx[2];
    ExtractJob(): job(0) {
        init_mutex(mutex);
        init_mutex(read_mutex);
    }
    ~ExtractJob() {
        destroy_mutex(mutex);
        destroy_mutex(read_mutex);
    }
};
struct Block {
	StringBuffer* in;
	enum { READY, WORKING, GOOD, BAD } state;
	int id, info;
	Block(int _id = -1, int _info = 0) : state(READY), id(_id), info(_info), in(new StringBuffer) {}

	void operator = (const Block& b) {
		in = b.in; state = b.state; id = b.id;
	}
};

struct ExtractJob {         // list of jobs
	Mutex mutex;              // protects state
	Mutex read_mutex;
	int job;                  // number of jobs started
	FILE *fp, *fout;
	uint32_t L, R;
	vector<Block> block;      // list of data blocks to extract
	vector<QuaBlock> binfo;
	vector<string>* qout;
	vector<int8_t> flag[2];
	vector<uint32_t> val[2];
	uint32_t cur[2];
	uint32_t mx[2];
	ExtractJob() : job(0) {
		init_mutex(mutex);
		init_mutex(read_mutex);
	}
	~ExtractJob() {
		destroy_mutex(mutex);
		destroy_mutex(read_mutex);
	}
};

void unmapping(StringBuffer& in, char score) {
	StringBuffer out;
	out.resize(BUFFER_SIZE + 1234);
	out.resize(0);
	int l2 = max(score - 7, 33), l3 = l2 + 4;
	int l4 = score + 7, l5 = l4 - 4;
	for (int i = 0, c = 0; (c = in.get()) != EOF;) {
		static int _ = 0;
		if (c == 0) { // end of line
			out.put(10);
			i = 0;
			continue;
		}
		else if (c >= 201 && c <= 255)
			while (c-->200) ++i, out.put(score);
		else if (c >= 137 && c <= 200) {
			c -= 137;
			out.put((c & 3) + l3);
			out.put(((c >> 2) & 3) + l3);
			out.put(((c >> 4) & 3) + l3);
			i += 3;
		}
		else if (c >= 73 && c <= 136) {
			c -= 73;
			out.put((c & 7) + l2);
			out.put(((c >> 3) & 7) + l2);
			i += 2;
		}
		else if (c >= 1 && c <= 72) {
			out.put(c + 32);
			++i;
		}
		else if (c >= 319 && c <= 382) {
			c -= 319;
			out.put((c & 3) - l5);
			out.put(((c >> 2) & 3) - l5);
			out.put(((c >> 4) & 3) - l5);
			i += 3;
		}
		else if (c >= 256 && c <= 319) {
			c -= 256;
			out.put((c & 7) - l4);
			out.put(((c >> 3) & 7) - l4);
			i += 2;
		}
	}
	out.swap(in);
}

char score;

// Decompress blocks in a job until none are READY
ThreadReturn decompressThread(void* arg) {
	ExtractJob& job = *(ExtractJob*)arg;

	int jobNumber = 0;

	// Get job number
	lock(job.mutex);
	jobNumber = ++job.job;
	release(job.mutex);

	// Look for next READY job.
	int next = 0;  // current job
	while (true) {
		lock(job.mutex);
		for (unsigned i = 0; i <= job.block.size(); ++i) {
			unsigned k = i + next;
			if (k >= job.block.size()) k -= job.block.size();
			if (i == job.block.size()) {  // no more jobs?
				release(job.mutex);
				return 0;
			}
			Block& b = job.block[k];
			if (b.state == Block::READY) {
				b.state = Block::WORKING;
				release(job.mutex);
				next = k;
				break;
			}
		}
		Block& b = job.block[next];
		QuaBlock& info = job.binfo[b.info];
		StringBuffer sb;
		sb.resize(info.length);
		lock(job.read_mutex);
		fseeko(job.fp, info.pos, SEEK_SET);
		fread(sb.data(), 1, info.length, job.fp);
		b.in->swap(sb);
		release(job.read_mutex);

		// Decompress
		StringBuffer out;
		Decompresser d;
		d.setInput(b.in);
		d.setOutput(&out);
		d.findBlock();
		d.findFilename();
		d.readComment();
		d.decompress();
		d.readSegmentEnd();
		delete b.in;
		b.in = NULL;


		// Write
		if (b.id == -1) {
			const auto& id = info.bucket;
			vector<string>& q = job.qout[id];
			size_t os = info.start;
			unmapping(out, score);
			char c;
			while ((c = out.get()) != EOF) {
				if (c != '\n') {
					q[os].push_back(c);
					while ((c = out.get()) != '\n' && c != EOF) q[os].push_back(c);
				}
				++os;
			}
			auto &_val = job.val[id];
			auto &_flag = job.flag[id];
			uint32_t pos = lower_bound(_val.begin(), _val.end(), info.end) - _val.begin();
			{
				uint32_t j = job.mx[id] + 1;
				while (j < pos && _flag[j] == 1) ++j;
				if (j == pos) {
					for (j = job.mx[id] + 1; j <= pos; ++j) _flag[j] = 2;
				}
				else _flag[pos] = 1;
			}
			if (_flag[pos] == 1) continue;
			if (id == 1) {
				job.mx[1] = pos;
				continue;
			}
			auto &val0 = job.val[0], &val1 = job.val[1];
			if (info.bucket == 0 && info.start <= job.L) {
				uint32_t temp = info.eline;
				for (uint32_t i = info.start; i < job.L; ++i) {
					if (job.qout[0][i].size() == 0) ++temp;
				}
				job.cur[1] = temp;
				job.cur[0] = job.L;
			}
			uint32_t l = job.cur[0];
			size_t sz = 0;
			while (job.cur[0] < val0[pos] && job.cur[1] < val1[job.mx[1]]) {
				if (job.qout[0][job.cur[0]].size() == 0) {
					job.qout[0][job.cur[0]].swap(job.qout[1][job.cur[1]++]);
				}
				sz += job.qout[0][job.cur[0]].size() + 1;
				++job.cur[0];
			}
			uint32_t r = min(job.cur[0], job.R);
			StringBuffer sb(sz);
			while (l < r) {
				auto &s = job.qout[0][l++];
				sb.write(s.c_str(), s.size());
				sb.put('\n');
				s.resize(0);
				string().swap(s);
			}
			fwrite(sb.c_str(), 1, sb.size(), job.fout);
			job.mx[0] = pos;
		}

	} // end while true

	  // Last block
	return 0;
}

decompressor::decompressor(int _threads) : threads(_threads), job(new ExtractJob) {
	if (threads < 1) threads = thread::hardware_concurrency();
	job->qout = qs_raw;
}


void decompressor::open(char *s) {
	job->fp = fopen(s, "rb");
}

void decompressor::set_out(const char *s) {
	job->fout = fopen(s, "w");
}

void decompressor::read_format() {
	fmt.read(job->fp);
	for (int i = 0; i < 2; ++i) qs_raw[i].resize(fmt.qlen[i]);
}

void decompressor::read_content() {
	for (size_t j = 0; j < job->binfo.size(); ++j) {
		QuaBlock& b = job->binfo[j];
		StringBuffer sb;
		qs_add(sb, j);
	}
}

void decompressor::read_table() {
	int64_t pos;
	fread(&pos, sizeof pos, 1, job->fp);
	fseeko(job->fp, pos, SEEK_SET);
	QuaBlock info;
	int cnt[2]{ 0 };
	while (info.read(job->fp)) {
		job->binfo.push_back(info);
		++cnt[info.bucket];
	}
	for (int i = 0; i < 2; ++i) job->flag[i].resize(cnt[i] + 1), job->flag[i][0] = 2;
	for (int i = 0; i < 2; ++i) job->val[i].resize(cnt[i] + 1), cnt[i] = 1;
	for (auto& _ : job->binfo) job->val[_.bucket][cnt[_.bucket]++] = _.end;
	for (auto& _ : job->val) sort(_.begin(), _.end());
	for (auto& _ : job->cur) _ = 0xffffffffu;
	for (auto& _ : job->mx) _ = 0;
}

void decompressor::qs_add(StringBuffer& q, int i) {
	job->block.push_back(Block(-1, i));
	job->block[job->block.size() - 1].in->swap(q);
}

void decompressor::start() {
	score = fmt.score;
	tid.resize(threads);
	for (unsigned i = 0; i<tid.size(); ++i) run(tid[i], decompressThread, job);
}

void decompressor::end() {
	for (unsigned i = 0; i<tid.size(); ++i) join(tid[i]);
	close();
}

void decompressor::close() {
	fclose(job->fp);
}
void decompressor::queue() {
	for (int i = 0; i < job->binfo.size(); i++) {
		for (int j = 0; j < job->binfo.size() - i - 1; j++) {
			if (job->binfo[j].start > job->binfo[j + 1].start) {
				QuaBlock temp = job->binfo[j];
				job->binfo[j] = job->binfo[j + 1];
				job->binfo[j + 1] = temp;
			}
		}
	}
	/*cout<<"job->binfo.size()="<<job->binfo.size()<<endl;
	for (int i = 0; i < job->binfo.size(); i++) {
	cout<<job->binfo[i].start<<",";
	}*/
}
void decompressor::get_qs() {
	job->L = 0; job->R = qs_raw[0].size();
	start();
	end();
	uint32_t a = job->cur[0], b = job->cur[1];
	StringBuffer sb;
	for (; a < job->R; ++a) {
		string &s = qs_raw[0][a].size() == 0 ? qs_raw[1][b++] : qs_raw[0][a];
		sb.write(s.c_str(), s.size());
		sb.put('\n');
	}
	fwrite(sb.c_str(), 1, sb.size(), job->fout);
	delete job;
}


/*void decompressor::void decompressor::get_block2(uint32_t L, uint32_t R) {
vector<bool> in(job->binfo.size(), false);
HashTable h = CreateTable(MAXTABLESIZE);
for (uint8_t i = 0; i < 2; ++i) {
for (size_t j = 0; j < job->binfo.size(); ++j) {
Quablock& b = job->binfo[j];
Insert(h, b.end);
Insert(h, b.start);
}
R = HashFind(h, L, R);
}
for (size_t j = 0; j < in.size(); ++j) if (in[j]) {
StringBuffer sb;
qs_add(sb, j);
}
}*/
void decompressor::get_block(uint32_t L, uint32_t R) {
	vector<bool> in(job->binfo.size(), false);
	for (uint8_t i = 0; i < 2; ++i) {
		uint32_t l = 0xffffffffu, r = 0;
		for (size_t j = 0; j < job->binfo.size(); ++j) {
			QuaBlock& b = job->binfo[j];
			if (i == b.bucket && b.end > L && b.start < R) {
				in[j] = true;
				l = min(l, b.eline);
				r = max(r, b.eline);
			}
		}
		auto &_val = job->val[i];
		job->mx[i] = lower_bound(_val.begin(), _val.end(), L + 1) - _val.begin() - 1;
		job->flag[i][job->mx[i]] = 2;
		L = l;
		l = r; r = fmt.qlen[(i + 1) % 2];
		for (auto& b : job->binfo) if (i == b.bucket) {
			if (b.eline > l && b.eline < r) r = b.eline;
		}
		R = r;
	}
	for (size_t j = 0; j < in.size(); ++j) if (in[j]) {
		StringBuffer sb;
		qs_add(sb, j);
	}
}
void decompressor::get_block_BinarySearch(uint32_t L, uint32_t R) {
	vector<bool> in(job->binfo.size(), false);
	int target_L = 0;
	int target_R = 2;
	int jobsize = job->binfo.size();
	int left_L = 0;
	int right_L = jobsize - 1;
	if (jobsize > 2) {
		while (left_L <= right_L) {
			int mid_L = (right_L + left_L) / 2;
			if ((L >= job->binfo[mid_L].start) && (L <= job->binfo[mid_L + 1].start)) {
				target_L = mid_L;
				left_L = jobsize - 1;
				right_L = 0;
			}
			else if (job->binfo[mid_L].start < L)
				left_L = mid_L + 1;
			else if (job->binfo[mid_L].start > L)
				right_L = mid_L - 1;
		}
	}
	int left_R = 0;
	int right_R = jobsize - 1;
	if (jobsize > 2) {
		while (left_R <= right_R) {
			int mid_R = (right_R + left_R) / 2;
			if ((L >= job->binfo[mid_R].start) && (L <= job->binfo[mid_R + 1].start)) {
				target_R = mid_R;
				left_R = jobsize;
				right_R = 0;
			}
			else if (job->binfo[mid_R].start < L)
				left_R = mid_R + 1;
			else if (job->binfo[mid_R].start > L)
				right_R = mid_R - 1;
		}
	}
	for (uint8_t m = 0; m < 2; ++m) {
		uint32_t l = 0xffffffffu, r = 0;
		for (size_t n = target_L; n < target_R; ++n) {
			QuaBlock& b = job->binfo[n];
			if (m == b.bucket) {
				in[n] = true;
				l = min(l, b.eline);
				r = max(r, b.eline);
			}
		}
		auto &_val = job->val[m];
		job->mx[m] = lower_bound(_val.begin(), _val.end(), L + 1) - _val.begin() - 1;
		job->flag[m][job->mx[m]] = 2;
		L = l;
		l = r; r = fmt.qlen[(m + 1) % 2];
		for (auto& b : job->binfo) if (m == b.bucket) {
			if (b.eline > l && b.eline < r) r = b.eline;
		}
		R = r;
	}
	for (size_t j = 0; j < in.size(); ++j) if (in[j]) {
		StringBuffer sb;
		qs_add(sb, j);
	}
}
void decompressor::query(uint32_t _L, uint32_t _R) {
	uint32_t L = _L - 1, R = _R;
	queue();

	get_block(L, R);
	//get_block_BinarySearch(L, R);
	job->L = L; job->R = R;
	start();
	end();
	uint32_t a = job->cur[0];
	uint32_t b = job->cur[1];
	StringBuffer sb;
	while (a < R) {
		string &s = qs_raw[0][a].size() == 0 ? qs_raw[1][b++] : qs_raw[0][a];
		sb.write(s.c_str(), s.size());
		sb.put('\n');
		++a;
	}
	fwrite(sb.c_str(), 1, sb.size(), job->fout);
	fclose(job->fout);
	delete job;
}

}
