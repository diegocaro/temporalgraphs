/*
 * count_ops.h
 *
 *  Created on: Nov 25, 2014
 *      Author: diegocaro
 */

#ifndef COUNT_OPS_H_
#define COUNT_OPS_H_

namespace count_ops {

#include <assert.h>

#ifdef COUNTOPS
//// Using Singleton Pattern
//// http://www.codeproject.com/Articles/1921/Singleton-Pattern-its-implementation-with-C
//// http://stackoverflow.com/questions/18654714/c-creating-global-reference-of-an-object
//class RamModel {
// public:
//    //RamModel(): _rank(0), _select(0), _access(0) {};
//    //RamModel() {}
//    void rank(size_t k=1) {
//        assert(k>0);
//        _rank+=k;
//    }
//    void access(size_t k=1) {
//            assert(k>0);
//            _access+=k;
//        }
//
//    void select(size_t k=1) {
//            assert(k>0);
//            _select+=k;
//        }
//
//    void print() {
//        printf("r/s/a\t%lu\t%lu\t%lu\n",_rank,_select,_access);
//    }
//
//    void clear() {
//        _rank = 0;
//        _select = 0;
//        _access = 0;
//    }
//
//    static RamModel* getInstance() {
//        if(! instanceFlag) {
//            single = new RamModel();
//            instanceFlag = true;
//            return single;
//        }
//        else {
//            return single;
//        }
//    }
//
//    ~RamModel() {
//        instanceFlag = false;
//    }
//
// private:
//    RamModel(): _rank(0), _select(0), _access(0) {};
//
//    size_t _rank;
//    size_t _select;
//    size_t _access;
//
//    static bool instanceFlag;
//    static RamModel *single;
//
//};
//bool RamModel::instanceFlag = false;
//RamModel* RamModel::single = NULL;

class RamModel {
public:
	RamModel(const char *name): _rank(0), _select(0), _access(0) {
		sprintf(_name, "%s", name);
	};

	void rank(size_t k=1) {
		assert(k>0);
		_rank+=k;
	}
	void access(size_t k=1) {
		assert(k>0);
		_access+=k;
	}

	void select(size_t k=1) {
		assert(k>0);
		_select+=k;
	}

	void print(const char *b="\n") {
		printf("%s.r\t%lu\t%s.s\t%lu\t%s.a\t%lu%s",_name,_rank,_name,_select,_name,_access,b);
	}

	void clear() {
		_rank = 0;
		_select = 0;
		_access = 0;
	}

private:
	size_t _rank;
	size_t _select;
	size_t _access;
	char _name[256];

};

#else

class RamModel {
public:
	RamModel(const char *name) {
		(void) name;
	}
	void rank(size_t k = 1) {
		(void) k;
	}
	void access(size_t k = 1) {
		(void) k;
	}
	void select(size_t k = 1) {
		(void) k;
	}
	void print(const char *b = "\n") {
		(void) b;
	}
	void clear() {
	}
};

#endif

static RamModel bitmapT("T");
static RamModel bitmapB("B");
static RamModel bitmapC("C");
}

#endif /* COUNT_OPS_H_ */
