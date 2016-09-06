/*
 * TemporalGraph.h
 *
 *  Created on: Apr 22, 2014
 *      Author: diegocaro
 */

#ifndef TEMPORALGRAPH_H_
#define TEMPORALGRAPH_H_

#include <sys/types.h>
#include <CompactQtree.h>
#include "arraysort.h"

using namespace cqtree_static;

// this is to remove duplicated items using hashing for pair<uint,uint> in an unordered_set
// moreinfo: http://stackoverflow.com/a/15161034/187284
#include <unordered_set>
#include <utility>
struct pair_hash {
   inline std::size_t operator()(const std::pair<int,int> & v) const {
       return v.first*31+v.second;
   }
};

enum TypeGraph {
  kInterval,
  kGrowth,
  kPoint,
  kIntervalGrowth,
  kIntervalPoint
};

enum typeds {
  ePRBlack, // compressed k^d-tree
  ePRWhite, // a failed experiment
  eMXDepth, // k^d-tree
  eMXFixed, // a failed experiment
  ePRB2Black, // bucket compressed k^d-tree
  ePRB2XORBlack // bucket xor compressed k^d-tree ... failed experiment
};

struct opts {
  BitSequenceBuilder *bs;  //bits for T bitmaps (tree)
  BitSequenceBuilder *bb;  //bits for B bitmaps (leaves)
  BitSequenceBuilder *bc;  //bits for C bitmaps (count leaves)

  enum typeds ds;  //type of compact qtree
  enum TypeGraph typegraph;

  const char *outfile;

  /* Name of input file.  May be "-".  */
  const char *infile;

  /* string parameters*/
  char params_char[255];
  map<string,string> params;

  int k1;
  int k2;
  int F;
  int lk1;  //levels for k1
  int lki;  //levels for ki
  int lf;  //levels for fixed mx

  //XorCodeBuilder *bx; //xor builder
};

#define TG_INTERV 3
#define TG_GROWTH 4
#define TG_POINT 5
#define TG_INTERVGROW 6
#define TG_INTERVPOINT 7




void readflags(struct opts *opts, const char *flags);
BitSequenceBuilder* getBSBuilder(string e);
uint *getBitmap(BitSequence *bs);
//XorCodeBuilder* getXorBuilder(string e);



// For updating the bitmaps data structures once is built
// This saves a lot of time for the MXCompactQtree (specially on compressed bitmaps!)

class UpdateMXCompactQtree: public MXCompactQtree {
    public:
  void updateBitmaps(BitSequenceBuilder *bt);
};

class UpdatePRBCompactQtree: public PRBCompactQtree {
    public:
  void updateBitmaps(BitSequenceBuilder *bt,BitSequenceBuilder *bb);
};

  class UpdatePRB2CompactQtree: public PRB2CompactQtree {
  public:
    void updateBitmaps(BitSequenceBuilder *bt,BitSequenceBuilder *bb, BitSequenceBuilder *bc);
};


class TemporalGraph {
 public:

  TemporalGraph() {
    qt_ = NULL;
    nodes_ = 0;
    edges_ = 0;
    lifetime_ = 0;
    contacts_ = 0;
  }

  static TemporalGraph* load(ifstream &f);

  virtual ~TemporalGraph() {
    if (qt_ != NULL) {
      delete qt_;
    }
    qt_ = NULL;
  }

  void setDs(CompactQtree *q) {
    qt_ = q;
  }

  void setInfo(uint nodes, uint edges, uint lifetime, uint contacts) {
    nodes_ = nodes;
    edges_ = edges;
    lifetime_ = lifetime;
    contacts_ = contacts;
  }

  unsigned getNodes() {
	  return nodes_;
  }


  virtual void stats() {
    qt_->stats_space();
  }

  void print_leaves() {


    if (dynamic_cast<PRBCompactQtree*>(qt_)) {
        ((PRBCompactQtree* )qt_ )->print_leaves();
    }
    else if (dynamic_cast<PRB2CompactQtree*>(qt_)) {
            ((PRB2CompactQtree* )qt_ )->print_leaves();
        }
    else {
        fprintf(stderr, "Input is not a PRB data structure\n");
    }
  }

  void updateCQBitmaps(CompactQtree *q, BitSequenceBuilder *bt, BitSequenceBuilder *bb, BitSequenceBuilder *bc) {
      if (dynamic_cast<PRBCompactQtree*>(q)) {
          ((UpdatePRBCompactQtree* )q )->updateBitmaps(bt,bb);
      }
      else if (dynamic_cast<PRB2CompactQtree*>(q)) {
              ((UpdatePRB2CompactQtree* )q )->updateBitmaps(bt,bb,bc);
          }
      else if (dynamic_cast<MXCompactQtree*>(q)) {
         ((UpdateMXCompactQtree* )q )->updateBitmaps(bt);
     }
      else {
          fprintf(stderr, "CompactQtree class cannot be updated to new bitmaps.\n");
          return;
      }
  }

  virtual void updateBitmaps(BitSequenceBuilder *bt, BitSequenceBuilder *bb, BitSequenceBuilder *bc) {
      updateCQBitmaps(qt_,bt,bb,bc);
  }

  virtual void save(ofstream &f)=0;

  //interface for quering a temporal graph
  virtual void direct_point(uint u, uint t, uint *res)=0;
  virtual void direct_weak(uint u, uint tstart, uint tend, uint *res)=0;
  virtual void direct_strong(uint u, uint tstart, uint tend, uint *res)=0;

  virtual void reverse_point(uint v, uint t, uint *res)=0;
  virtual void reverse_weak(uint v, uint tstart, uint tend, uint *res)=0;
  virtual void reverse_strong(uint v, uint tstart, uint tend, uint *res)=0;

  virtual int edge_point(uint u, uint v, uint t)=0;
  virtual int edge_weak(uint u, uint v, uint tstart, uint tend)=0;
  virtual int edge_strong(uint u, uint v, uint tstart, uint tend)=0;
  virtual int edge_next(uint u, uint v, uint t)=0;

  virtual unsigned long snapshot(uint t)=0;
  virtual unsigned long contacts()=0;

  virtual unsigned long change_point(uint t)=0;
  virtual unsigned long change_interval(uint tstart, uint tend)=0;
  virtual unsigned long actived_point(uint t)=0;
  virtual unsigned long actived_interval(uint tstart, uint tend)=0;
  virtual unsigned long deactived_point(uint t)=0;
  virtual unsigned long deactived_interval(uint tstart, uint tend)=0;


 protected:
  uint nodes_;
  uint edges_;
  uint lifetime_;
  uint contacts_;

  CompactQtree *qt_;
  vector<Point<uint> > vp;
};

class IntervalContactGraph : public TemporalGraph {
 public:
  IntervalContactGraph() {
  }
  ;

  ~IntervalContactGraph() {
    if (qt_ != NULL) {
      delete qt_;
    }
    qt_ = NULL;
  }
  ;

  // Load & Save
  IntervalContactGraph(ifstream &f) {
    uint type = loadValue<uint>(f);
    // TODO:throw an exception!
    if (type != TG_INTERV) {
      abort();
    }

    loadValue(f, nodes_);
    loadValue(f, edges_);
    loadValue(f, lifetime_);
    loadValue(f, contacts_);
    //loadValue(f, opts_);
    qt_ = CompactQtree::load(f);
  }

  void save(ofstream &f) {
    uint wr = TG_INTERV;
    saveValue(f, wr);
    saveValue(f, nodes_);
    saveValue(f, edges_);
    saveValue(f, lifetime_);
    saveValue(f, contacts_);
    //saveValue(f, opts_);

    qt_->save(f);
  }

  /// Interface

  virtual void direct_point(uint u, uint t, uint *res) {
    direct_strong(u, t, t+1, res);
  }

  virtual void direct_weak(uint u, uint tstart, uint tend, uint *res) {
    vp.clear();

    Point<uint> from(4);
      Point<uint> to(4);

      from[0] = u;
      from[1] = 0;
      from[2] = 0;
      from[3] = tstart+1;

      to[0] = u+1;
      to[1] = nodes_;
      to[2] = tend;
      to[3] = lifetime_;



//#ifdef EXPERIMENTS
//      *res += qt_->range(from,to,vp,false);
//#else
      *res += qt_->range(from,to,vp,false);
	return;      
  //*res = vp.size();
      for (size_t i = 0; i < vp.size(); i++) {
        res[i + 1 + *res] = vp[i][1];
      }
      *res += vp.size();
//#endif

      qsort(&res[1], *res, sizeof(unsigned int), compare);
      remove_duplicates(res);

  }
  virtual void direct_strong(uint u, uint tstart, uint tend, uint *res) {
    vp.clear();

    Point<uint> from(4);
      Point<uint> to(4);

      from[0] = u;
      from[1] = 0;
      from[2] = 0;
      from[3] = tend;

      to[0] = u+1;
      to[1] = nodes_;
      to[2] = tstart+1;
      to[3] = lifetime_;

#ifdef EXPERIMENTS
      *res += qt_->range(from,to,vp,false);
#else
      qt_->range(from,to,vp,true);
        //*res = vp.size();
      for (size_t i = 0; i < vp.size(); i++) {
        res[i + 1 + *res] = vp[i][1];
      }
      *res += vp.size();
#endif
  }

  virtual void reverse_point(uint v, uint t, uint *res) {
    reverse_strong(v, t, t+1, res);
  }

  virtual void reverse_weak(uint v, uint tstart, uint tend, uint *res) {
    vp.clear();

    Point<uint> from(4);
      Point<uint> to(4);

      from[0] = 0;
      from[1] = v;
      from[2] = 0;
      from[3] = tstart+1;

      to[0] = nodes_;
      to[1] = v+1;
      to[2] = tend;
      to[3] = lifetime_;

//#ifdef EXPERIMENTS
//      *res += qt_->range(from,to,vp,false);
//#else
      qt_->range(from,to,vp,true);
        //*res = vp.size();
      for (size_t i = 0; i < vp.size(); i++) {
        res[i + 1 + *res] = vp[i][0];
      }
      *res += vp.size();
      qsort(&res[1], *res, sizeof(unsigned int), compare);
      remove_duplicates(res);
//#endif
  }
  virtual void reverse_strong(uint v, uint tstart, uint tend, uint *res) {
    vp.clear();

    Point<uint> from(4);
      Point<uint> to(4);

      from[0] = 0;
      from[1] = v;
      from[2] = 0;
      from[3] = tend;

      to[0] = nodes_;
      to[1] = v+1;
      to[2] = tstart+1;
      to[3] = lifetime_;

#ifdef EXPERIMENTS
      *res = qt_->range(from,to,vp,false);
#else
      qt_->range(from,to,vp,true);
        //*res = vp.size();
      for (size_t i = 0; i < vp.size(); i++) {
        res[i + 1 + *res] = vp[i][0];
      }
      *res += vp.size();
#endif
  }

  virtual int edge_point(uint u, uint v, uint t) {
    return edge_strong(u,v,t,t+1);
  }

  virtual int edge_weak(uint u, uint v, uint tstart, uint tend) {
    vp.clear();

    Point<uint> from(4);
      Point<uint> to(4);

      from[0] = u;
      from[1] = v;
      from[2] = 0;
      from[3] = tstart+1;

      to[0] = u+1;
      to[1] = v+1;
      to[2] = tend;
      to[3] = lifetime_;

      qt_->range(from,to,vp,true);

//      for(int i=0; i < vp.size();i++) {
//          vp[i].print();
//      }


      if (vp.size() > 0) return 1;
      return 0;
  }
  virtual int edge_strong(uint u, uint v, uint tstart, uint tend) {
    vp.clear();

    Point<uint> from(4);
      Point<uint> to(4);

      from[0] = u;
      from[1] = v;
      from[2] = 0;
      from[3] = tend;

      to[0] = u+1;
      to[1] = v+1;
      to[2] = tstart+1;
      to[3] = lifetime_;

      qt_->range(from,to,vp,true);
      if (vp.size() > 0) return 1;
      return 0;
  }
  virtual int edge_next(uint u, uint v, uint t) {
	  if (edge_point(u,v,t)) return t;
	  
	  edge_weak(u,v,t,lifetime_);
	  if (vp.size() > 0) {
	      return vp[0][3];
	  }
	  
	  return -1;
  }

  virtual unsigned long snapshot(uint t) {
      vp.clear();
    Point<uint> from(4);
      Point<uint> to(4);

      from[0] = 0;
      from[1] = 0;
      from[2] = 0;
      from[3] = t+1;

      to[0] = nodes_;
      to[1] = nodes_;
      to[2] = t+1;
      to[3] = lifetime_;

      return qt_->range(from,to,vp,false);
  }

  virtual unsigned long contacts() {
      vp.clear();
    Point<uint> from(4);
      Point<uint> to(4);

      from[0] = 0;
      from[1] = 0;
      from[2] = 0;
      from[3] = 0;

      to[0] = nodes_;
      to[1] = nodes_;
      to[2] = lifetime_;
      to[3] = lifetime_;

      return qt_->range(from,to,vp,false);
  }

  virtual unsigned long change_point(uint t) {
      return change_interval(t,t+1);
  }


  virtual unsigned long change_interval(uint tstart, uint tend) {
      actived_interval_(tstart,tend,false,true);
      return deactived_interval_(tstart,tend,false,false);
  }

  unsigned long actived_interval_(uint tstart, uint tend, bool allow_duplicated, bool clear_vp=true) {
      if (clear_vp) vp.clear();
      Point<uint> from(4);
      Point<uint> to(4);

      from[0] = 0;
      from[1] = 0;
      from[2] = tstart;
      from[3] = 0; // can be "tstart+1", but due the temporal interval we can do the same with "0"

      to[0] = nodes_;
      to[1] = nodes_;
      to[2] = tend;
      to[3] = lifetime_;

      //returning duplicated entries
      if (allow_duplicated == true) {
          return qt_->range(from,to,vp,false);
      }

      //removing duplicated
      unordered_set< pair<uint,uint>,  pair_hash > ans;

      qt_->range(from,to,vp,true);
      for (size_t i = 0; i < vp.size(); i++) {
          ans.insert(make_pair(vp[i][0],vp[i][1]));
      }
      return ans.size();
  }

  virtual unsigned long actived_point(uint t) {
      return actived_interval_(t,t+1,true);
  }

  virtual unsigned long actived_interval(uint tstart, uint tend) {
      return actived_interval_(tstart,tend,false);
  }


  virtual unsigned long deactived_interval_(uint tstart, uint tend, bool allow_duplicated, bool clear_vp=true) {
      if (clear_vp) vp.clear();
      Point<uint> from(4);
      Point<uint> to(4);

      from[0] = 0;
      from[1] = 0;
      from[2] = 0;
      from[3] = tstart;

      to[0] = nodes_;
      to[1] = nodes_;
      to[2] = lifetime_; // can be "tend-1", but due the temporal interval we can do the same with "lifetime_"
      to[3] = tend;

      //returning duplicated entries
      if (allow_duplicated == true) {
          return qt_->range(from,to,vp,false);
      }

      //removing duplicated
      unordered_set< pair<uint,uint>,  pair_hash > ans;

      qt_->range(from,to,vp,true);
      for (size_t i = 0; i < vp.size(); i++) {
          ans.insert(make_pair(vp[i][0],vp[i][1]));
      }
      return ans.size();
  }
  virtual unsigned long deactived_point(uint t) {
      return deactived_interval_(t,t+1,true);
  }

  virtual unsigned long deactived_interval(uint tstart, uint tend) {
      return deactived_interval_(tstart,tend,false);
  }

};

class GrowingContactGraph : public TemporalGraph {
 public:
  GrowingContactGraph() {
  }
  ;

  ~GrowingContactGraph() {
    if (qt_ != NULL) {
      delete qt_;
    }
    qt_ = NULL;
  }
  ;

  // Load & Save
  GrowingContactGraph(ifstream &f) {
    uint type = loadValue<uint>(f);
    // TODO:throw an exception!
    if (type != TG_GROWTH) {
      abort();
    }

    loadValue(f, nodes_);
    loadValue(f, edges_);
    loadValue(f, lifetime_);
    loadValue(f, contacts_);
    //loadValue(f, opts_);
    qt_ = CompactQtree::load(f);
  }

  void save(ofstream &f) {
    uint wr = TG_GROWTH;
    saveValue(f, wr);
    saveValue(f, nodes_);
    saveValue(f, edges_);
    saveValue(f, lifetime_);
    saveValue(f, contacts_);
    //saveValue(f, opts_);

    qt_->save(f);
  }

  /// Interface

  virtual void direct_point(uint u, uint t, uint *res) {
    vp.clear();

    Point<uint> from(3);
      Point<uint> to(3);

      from[0] = u;
      from[1] = 0;
      from[2] = 0;


      to[0] = u+1;
      to[1] = nodes_;
      to[2] = t+1;

#ifdef EXPERIMENTS
      *res += qt_->range(from,to,vp,false);
#else
      qt_->range(from,to,vp,true);
      //*res = vp.size();
          for (size_t i = 0; i < vp.size(); i++) {
            res[i + 1 + *res] = vp[i][1];
          }
          *res += vp.size();
      #endif
  }
  virtual void direct_strong(uint u, uint tstart, uint tend, uint *res) {
      UNUSED(tend);
    direct_point(u,tstart,res);
  }

  virtual void direct_weak(uint u, uint tstart, uint tend, uint *res) {
      UNUSED(tstart);
    direct_point(u,tend,res);
  }

  virtual void reverse_point(uint v, uint t, uint *res) {
    vp.clear();

    Point<uint> from(3);
      Point<uint> to(3);

      from[0] = 0;
      from[1] = v;
      from[2] = 0;


      to[0] = nodes_;
      to[1] = v+1;
      to[2] = t+1;

#ifdef EXPERIMENTS
      *res += qt_->range(from,to,vp,false);
#else
      qt_->range(from,to,vp,true);
      //*res = vp.size();
          for (size_t i = 0; i < vp.size(); i++) {
            res[i + 1 + *res] = vp[i][0];
          }
          *res += vp.size();
      #endif
  }

  virtual void reverse_weak(uint v, uint tstart, uint tend, uint *res) {
      UNUSED(tstart);
    reverse_point(v,tend,res);
  }
  virtual void reverse_strong(uint v, uint tstart, uint tend, uint *res) {
      UNUSED(tend);
    reverse_point(v,tstart,res);
  }

  virtual int edge_point(uint u, uint v, uint t) {
    vp.clear();

    Point<uint> from(3);
      Point<uint> to(3);

      from[0] = u;
      from[1] = v;
      from[2] = 0;


      to[0] = u+1;
      to[1] = v+1;
      to[2] = t+1;

      qt_->range(from,to,vp,true);
      if (vp.size() > 0 ) return 1;
      return 0;
  }

  virtual int edge_weak(uint u, uint v, uint tstart, uint tend) {
      UNUSED(tstart);
    return edge_point(u,v,tend);
  }
  virtual int edge_strong(uint u, uint v, uint tstart, uint tend) {
      UNUSED(tend);
    return edge_point(u,v,tstart);
  }

  virtual int edge_next(uint u, uint v, uint t) {
      if (edge_point(u,v,t)) return t;

      edge_weak(u,v,t,lifetime_);
      if (vp.size() > 0) {
          return vp[0][3];
      }

      return -1;
  }

  virtual unsigned long snapshot(uint t) {
    vp.clear();

    Point<uint> from(3);
      Point<uint> to(3);

      from[0] = 0;
      from[1] = 0;
      from[2] = 0;


      to[0] = nodes_;
      to[1] = nodes_;
      to[2] = t+1;

      return qt_->range(from,to,vp,0);
  }

  virtual unsigned long contacts() {
      vp.clear();

      Point<uint> from(3);
        Point<uint> to(3);

        from[0] = 0;
        from[1] = 0;
        from[2] = 0;


        to[0] = nodes_;
        to[1] = nodes_;
        to[2] = lifetime_;

      return qt_->range(from,to,vp,false);
  }

  virtual unsigned long change_point(uint t) {
      return change_interval(t,t+1);
  }

  virtual unsigned long change_interval(uint tstart, uint tend) {
      return actived_interval(tstart,tend) + deactived_interval(tstart,tend);
  }
  virtual unsigned long actived_point(uint t) {
      return actived_interval(t,t+1);
  }
  virtual unsigned long actived_interval(uint tstart, uint tend) {
      vp.clear();

      Point<uint> from(3);
        Point<uint> to(3);

        from[0] = 0;
        from[1] = 0;
        from[2] = tstart;


        to[0] = nodes_;
        to[1] = nodes_;
        to[2] = tend;

      return qt_->range(from,to,vp,false);
  }
  virtual unsigned long deactived_point(uint t) {
      return deactived_interval(t,t+1);
  }
  virtual unsigned long deactived_interval(uint tstart, uint tend) {
      UNUSED(tstart);
      vp.clear();

      if (tend < lifetime_-1) {
          return 0;
      }

      return snapshot(tend);
  }




};

class PointContactGraph : public TemporalGraph {
 public:

  PointContactGraph() {
  }
  ;

  ~PointContactGraph() {
    if (qt_ != NULL) {
      delete qt_;
    }
    qt_ = NULL;
  }
  ;

  // Load & Save
  PointContactGraph(ifstream &f) {
    uint type = loadValue<uint>(f);
    // TODO:throw an exception!
    if (type != TG_POINT) {
      abort();
    }

    loadValue(f, nodes_);
    loadValue(f, edges_);
    loadValue(f, lifetime_);
    loadValue(f, contacts_);
    //loadValue(f, opts_);
    qt_ = CompactQtree::load(f);
  }

  void save(ofstream &f) {
    uint wr = TG_POINT;
    saveValue(f, wr);
    saveValue(f, nodes_);
    saveValue(f, edges_);
    saveValue(f, lifetime_);
    saveValue(f, contacts_);
    //saveValue(f, opts_);

    qt_->save(f);
  }

  /// Interface

  virtual void direct_point(uint u, uint t, uint *res) {
    direct_weak(u,t,t+1,res);
  }

  virtual void direct_weak(uint u, uint tstart, uint tend, uint *res) {
    vp.clear();

    Point<uint> from(3);
      Point<uint> to(3);

      from[0] = u;
      from[1] = 0;
      from[2] = tstart;


      to[0] = u+1;
      to[1] = nodes_;
      to[2] = tend;

//#ifdef EXPERIMENTS
//      *res += qt_->range(from,to,vp,false);
//#else
      qt_->range(from,to,vp,true);
      //*res = vp.size();
          for (size_t i = 0; i < vp.size(); i++) {
            res[i + 1 + *res] = vp[i][1];
          }
          *res += vp.size();
//     #endif

          qsort(&res[1], *res, sizeof(unsigned int), compare);
          remove_duplicates(res);
  }
  virtual void direct_strong(uint u, uint tstart, uint tend, uint *res) {
    if (tstart+1 == tend) {
      direct_point(u,tstart,res);
    }
    else {
      *res = 0;
    }

  }

  virtual void reverse_point(uint v, uint t, uint *res) {
    reverse_weak(v,t,t+1,res);
  }

  virtual void reverse_weak(uint v, uint tstart, uint tend, uint *res) {
    vp.clear();

    Point<uint> from(3);
      Point<uint> to(3);

      from[0] = 0;
      from[1] = v;
      from[2] = tstart;


      to[0] = nodes_;
      to[1] = v+1;
      to[2] = tend;

//#ifdef EXPERIMENTS
//      *res += qt_->range(from,to,vp,false);
//#else
      qt_->range(from,to,vp,true);
      //*res = vp.size();
          for (size_t i = 0; i < vp.size(); i++) {
            res[i + 1 + *res] = vp[i][0];
          }
          *res += vp.size();
//#endif
      qsort(&res[1], *res, sizeof(unsigned int), compare);
      remove_duplicates(res);
  }
  virtual void reverse_strong(uint v, uint tstart, uint tend, uint *res) {
    if (tstart+1 == tend) {
      reverse_point(v,tstart,res);
    }
    else {
      *res = 0;
    }
  }

  virtual int edge_point(uint u, uint v, uint t) {
   return edge_weak(u,v,t,t+1);
  }

  virtual int edge_weak(uint u, uint v, uint tstart, uint tend) {
    vp.clear();

    Point<uint> from(3);
      Point<uint> to(3);

      from[0] = u;
      from[1] = v;
      from[2] = tstart;


      to[0] = u+1;
      to[1] = v+1;
      to[2] = tend;

      qt_->range(from,to,vp,true);
      if (vp.size() > 0) return 1;
      else return 0;
  }
  virtual int edge_strong(uint u, uint v, uint tstart, uint tend) {
    if (tstart+1 == tend) {
      return edge_point(u,v,tstart);
    }
    return 0;
  }
  virtual int edge_next(uint u, uint v, uint t) {
      if (edge_point(u,v,t)) return t;

      edge_weak(u,v,t,lifetime_);
      if (vp.size() > 0) {
          return vp[0][3];
      }

      return -1;
  }


  virtual unsigned long snapshot(uint t) {
    vp.clear();

    Point<uint> from(3);
      Point<uint> to(3);

      from[0] = 0;
      from[1] = 0;
      from[2] = t;


      to[0] = nodes_;
      to[1] = nodes_;
      to[2] = t+1;

     return qt_->range(from,to,vp,false);

  }

  virtual unsigned long contacts() {
      vp.clear();

      Point<uint> from(3);
        Point<uint> to(3);

        from[0] = 0;
        from[1] = 0;
        from[2] = 0;


        to[0] = nodes_;
        to[1] = nodes_;
        to[2] = lifetime_;

       return  qt_->range(from,to,vp,false);

  }



  virtual unsigned long change_point(uint t) {
      return change_interval(t,t+1);
  }

  virtual unsigned long change_interval(uint tstart, uint tend) {
      actived_interval_(tstart,tend,false,true);
            return deactived_interval_(tstart,tend,false,false);
  }
  virtual unsigned long actived_point(uint t) {
      return actived_interval_(t,t+1, true);
  }

  virtual unsigned long actived_interval(uint tstart, uint tend) {
      return actived_interval_(tstart,tend,false);
  }


  unsigned long actived_interval_(uint tstart, uint tend, bool allow_duplicated, bool clear_vp=true) {
      if (clear_vp) vp.clear();

      Point<uint> from(3);
        Point<uint> to(3);

        from[0] = 0;
        from[1] = 0;
        from[2] = tstart;


        to[0] = nodes_;
        to[1] = nodes_;
        to[2] = tend;

        //returning duplicated entries
        if (allow_duplicated == true) {
            return qt_->range(from,to,vp,false);
        }

        //removing duplicated
        unordered_set< pair<uint,uint>,  pair_hash > ans;

        qt_->range(from,to,vp,true);
        for (size_t i = 0; i < vp.size(); i++) {
            ans.insert(make_pair(vp[i][0],vp[i][1]));
        }
        return ans.size();
  }
  virtual unsigned long deactived_point(uint t) {
      return deactived_interval(t,t+1);
  }

  virtual unsigned long deactived_interval(uint tstart, uint tend) {
      return deactived_interval_(tstart,tend,false);

  }

  unsigned long deactived_interval_(uint tstart, uint tend, bool allow_duplicated, bool clear_vp=true) {
      if (clear_vp) vp.clear();

       Point<uint> from(3);
         Point<uint> to(3);

         from[0] = 0;
         from[1] = 0;
         from[2] = tstart-1;


         to[0] = nodes_;
         to[1] = nodes_;
         to[2] = tend-1;

         //returning duplicated entries
         if (allow_duplicated == true) {
             return qt_->range(from,to,vp,false);
         }

         //removing duplicated
         unordered_set< pair<uint,uint>,  pair_hash > ans;

         qt_->range(from,to,vp,true);
         for (size_t i = 0; i < vp.size(); i++) {
             ans.insert(make_pair(vp[i][0],vp[i][1]));
         }
         return ans.size();
  }





};

// @TODO this can be improved..., is ugly the way I mixed up the classes
// to get an itnerval and growing graph
class IntervalContactGraphGrowth : public TemporalGraph {
 public:
    IntervalContactGraphGrowth() {
  }
  ;

  ~IntervalContactGraphGrowth() {
      if (past_!=NULL) delete past_;
      if (curr_!=NULL) delete curr_;
      past_=NULL;
      curr_=NULL;
  }
  ;

  // Load & Save
  IntervalContactGraphGrowth(ifstream &f) {
    uint type = loadValue<uint>(f);
    // TODO:throw an exception!
    if (type != TG_INTERVGROW) {
      abort();
    }

    loadValue(f, nodes_);
    loadValue(f, edges_);
    loadValue(f, lifetime_);
    loadValue(f, contacts_);
    //loadValue(f, opts_);

    past_ = new IntervalContactGraph(f);
    curr_ = new GrowingContactGraph(f);

    qt_ = NULL;
  }

  void save(ofstream &f) {
    uint wr = TG_INTERVGROW;
    saveValue(f, wr);
    saveValue(f, nodes_);
    saveValue(f, edges_);
    saveValue(f, lifetime_);
    saveValue(f, contacts_);
    //saveValue(f, opts_);

    past_->save(f);
    curr_->save(f);
  }

 void setGraphs(IntervalContactGraph *past, GrowingContactGraph *curr) {
    past_ = past;
    curr_ = curr;

}

 virtual void updateBitmaps(BitSequenceBuilder *bt, BitSequenceBuilder *bb, BitSequenceBuilder *bc) {
     past_->updateBitmaps(bt,bb,bc);
     curr_->updateBitmaps(bt,bb,bc);
 }
 
 virtual void stats() {
   past_->stats();
   curr_->stats();
 }

  /// Interface

  virtual void direct_point(uint u, uint t, uint *res) {
    past_->direct_point(u,t,res);
    curr_->direct_point(u,t,res);
  }

  virtual void direct_weak(uint u, uint tstart, uint tend, uint *res) {
      past_->direct_weak(u,tstart,tend,res);
      curr_->direct_weak(u,tstart,tend,res);

      qsort(&res[1], *res, sizeof(unsigned int), compare);
      remove_duplicates(res);
  }
  virtual void direct_strong(uint u, uint tstart, uint tend, uint *res) {
      past_->direct_strong(u,tstart,tend,res);
      curr_->direct_strong(u,tstart,tend,res);

      qsort(&res[1], *res, sizeof(unsigned int), compare);
      remove_duplicates(res);
  }

  virtual void reverse_point(uint v, uint t, uint *res) {
      past_->reverse_point(v,t,res);
      curr_->reverse_point(v,t,res);
  }

  virtual void reverse_weak(uint v, uint tstart, uint tend, uint *res) {
    past_->reverse_weak(v,tstart,tend,res);
    curr_->reverse_weak(v,tstart,tend,res);

    qsort(&res[1], *res, sizeof(unsigned int), compare);
    remove_duplicates(res);
  }
  virtual void reverse_strong(uint v, uint tstart, uint tend, uint *res) {
    past_->reverse_strong(v,tstart,tend,res);
    curr_->reverse_strong(v,tstart,tend,res);

    qsort(&res[1], *res, sizeof(unsigned int), compare);
    remove_duplicates(res);
  }

  virtual int edge_point(uint u, uint v, uint t) {
    return (past_->edge_point(u,v,t) || curr_->edge_point(u,v,t));
  }

  virtual int edge_weak(uint u, uint v, uint tstart, uint tend) {
      return (past_->edge_weak(u,v,tstart,tend) || curr_->edge_weak(u,v,tstart,tend));
  }
  virtual int edge_strong(uint u, uint v, uint tstart, uint tend) {
      return (past_->edge_strong(u,v,tstart,tend) || curr_->edge_strong(u,v,tstart,tend));
  }
  virtual int edge_next(uint u, uint v, uint t) {
    int p = past_->edge_next(u,v,t);
    int c = curr_->edge_next(u,v,t);

    if (p >= 0 && p < c) return p;
    return c;
  }

  virtual unsigned long snapshot(uint t) {
      return past_->snapshot(t) + curr_->snapshot(t);
  }

  virtual unsigned long contacts() {
      return past_->contacts() + curr_->contacts();
  }


  virtual unsigned long change_point(uint t) {
      return past_->change_point(t) + curr_->change_point(t);
  }

  virtual unsigned long change_interval(uint tstart, uint tend) {
      return past_->change_interval(tstart,tend) + curr_->change_interval(tstart,tend);
  }

  virtual unsigned long actived_point(uint t) {
      return past_->actived_point(t) + curr_->actived_point(t);
  }

  virtual unsigned long actived_interval(uint tstart, uint tend) {
      return past_->actived_interval(tstart,tend) + curr_->actived_interval(tstart,tend);
  }

  virtual unsigned long deactived_point(uint t) {
      return past_->deactived_point(t) + curr_->deactived_point(t);
  }

  virtual unsigned long deactived_interval(uint tstart, uint tend) {
      return past_->deactived_interval(tstart,tend) + curr_->deactived_interval(tstart,tend);
  }


  IntervalContactGraph *past_;
  GrowingContactGraph *curr_;

};

// @TODO this can be improved..., is ugly the way I mixed up the classes
// to get an itnerval and point graph
class IntervalContactGraphPoint : public TemporalGraph {
 public:
    IntervalContactGraphPoint() {
  }
  ;

  ~IntervalContactGraphPoint() {
      if (interval_!=NULL) delete interval_;
      if (point_!=NULL) delete point_;
      interval_=NULL;
      point_=NULL;
  }
  ;

  // Load & Save
  IntervalContactGraphPoint(ifstream &f) {
    uint type = loadValue<uint>(f);
    // TODO:throw an exception!
    if (type != TG_INTERVPOINT) {
      abort();
    }

    loadValue(f, nodes_);
    loadValue(f, edges_);
    loadValue(f, lifetime_);
    loadValue(f, contacts_);
    //loadValue(f, opts_);

    interval_ = new IntervalContactGraph(f);
    point_ = new PointContactGraph(f);

    qt_ = NULL;
  }

  void save(ofstream &f) {
    uint wr = TG_INTERVPOINT;
    saveValue(f, wr);
    saveValue(f, nodes_);
    saveValue(f, edges_);
    saveValue(f, lifetime_);
    saveValue(f, contacts_);
    //saveValue(f, opts_);

    interval_->save(f);
    point_->save(f);
  }

 void setGraphs(IntervalContactGraph *interval, PointContactGraph *point) {
    interval_ = interval;
    point_ = point;

}

 virtual void updateBitmaps(BitSequenceBuilder *bt, BitSequenceBuilder *bb, BitSequenceBuilder *bc) {
     interval_->updateBitmaps(bt,bb,bc);
     point_->updateBitmaps(bt,bb,bc);
 }
 
 virtual void stats() {
   interval_->stats();
   point_->stats();
 }
 
  /// Interface

  virtual void direct_point(uint u, uint t, uint *res) {
    interval_->direct_point(u,t,res);
    point_->direct_point(u,t,res);
  }

  virtual void direct_weak(uint u, uint tstart, uint tend, uint *res) {
      interval_->direct_weak(u,tstart,tend,res);
      point_->direct_weak(u,tstart,tend,res);
  }
  virtual void direct_strong(uint u, uint tstart, uint tend, uint *res) {
      interval_->direct_strong(u,tstart,tend,res);
      point_->direct_strong(u,tstart,tend,res);
  }

  virtual void reverse_point(uint v, uint t, uint *res) {
      interval_->reverse_point(v,t,res);
      point_->reverse_point(v,t,res);
  }

  virtual void reverse_weak(uint v, uint tstart, uint tend, uint *res) {
    interval_->reverse_weak(v,tstart,tend,res);
    point_->reverse_weak(v,tstart,tend,res);
  }
  virtual void reverse_strong(uint v, uint tstart, uint tend, uint *res) {
    interval_->reverse_strong(v,tstart,tend,res);
    point_->reverse_strong(v,tstart,tend,res);
  }

  virtual int edge_point(uint u, uint v, uint t) {
    return (interval_->edge_point(u,v,t) || point_->edge_point(u,v,t));
  }

  virtual int edge_weak(uint u, uint v, uint tstart, uint tend) {
      return (interval_->edge_weak(u,v,tstart,tend) || point_->edge_weak(u,v,tstart,tend));
  }
  virtual int edge_strong(uint u, uint v, uint tstart, uint tend) {
      return (interval_->edge_strong(u,v,tstart,tend) || point_->edge_strong(u,v,tstart,tend));
  }
  virtual int edge_next(uint u, uint v, uint t) {
    int p = interval_->edge_next(u,v,t);
    int c = point_->edge_next(u,v,t);

    if (p >= 0 && p < c) return p;
    return c;
  }

  virtual unsigned long snapshot(uint t) {
      return interval_->snapshot(t) + point_->snapshot(t);
  }

  virtual unsigned long contacts() {
      return interval_->contacts() + point_->contacts();
  }

  virtual unsigned long change_point(uint t) {
      return interval_->change_point(t) + point_->change_point(t);
  }

  virtual unsigned long change_interval(uint tstart, uint tend) {
      return interval_->change_interval(tstart,tend) + point_->change_interval(tstart,tend);
  }

  virtual unsigned long actived_point(uint t) {
      return interval_->actived_point(t) + point_->actived_point(t);
  }

  virtual unsigned long actived_interval(uint tstart, uint tend) {
      return interval_->actived_interval(tstart,tend) + point_->actived_interval(tstart,tend);
  }

  virtual unsigned long deactived_point(uint t) {
      return interval_->deactived_point(t) + point_->deactived_point(t);
  }

  virtual unsigned long deactived_interval(uint tstart, uint tend) {
      return interval_->deactived_interval(tstart,tend) + point_->deactived_interval(tstart,tend);
  }



  IntervalContactGraph *interval_;
  PointContactGraph *point_;

};


#endif /* TEMPORALGRAPH_H_ */
