/*
 * TemporalGraph.cc
 *
 *  Created on: Sep 2, 2014
 *      Author: diegocaro
 */

#include "TemporalGraph.h"



TemporalGraph* TemporalGraph::load(ifstream &fp) {
    uint r = loadValue<uint>(fp);
    size_t pos = fp.tellg();
    fp.seekg(pos-sizeof(uint));
    switch(r) {
      case TG_INTERV: return new IntervalContactGraph(fp);
      case TG_GROWTH: return new GrowingContactGraph(fp);
      case TG_POINT: return new PointContactGraph(fp);
      case TG_INTERVGROW: return new IntervalContactGraphGrowth(fp);
      case TG_INTERVPOINT: return new IntervalContactGraphPoint(fp);
    }
    return NULL;
  }

void readflags(struct opts *opts, const char *flags) {
    strcpy(opts->params_char,flags);
    vector<string> s;
    tokenize(flags,s,',');

    for(size_t i = 0; i < s.size(); i++) {
        vector<string> u;
        tokenize(s[i],u,':');
        opts->params[u[0]] = u[1];
    }

    opts->k1 = atoi(opts->params["k1"].c_str());
    opts->k2 = atoi(opts->params["k2"].c_str());
    opts->lk1 = atoi(opts->params["lk1"].c_str());
    opts->lki = atoi(opts->params["lki"].c_str());
    opts->lf = atoi(opts->params["lf"].c_str());
    opts->F = atoi(opts->params["F"].c_str());
}

BitSequenceBuilder* getBSBuilder(string e) {
    BitSequenceBuilder *bs=NULL;
    //factor=2 => overhead 50%
    //factor=3 => overhead 33%
    //factor=4 => overhead 25%
    //factor=20=> overhead 5%
    if (e == "RG5") {
        bs = new BitSequenceBuilderRG(20);  // by default, 5% of extra space for bitmaps -- cada 20*32=640 bits
    } else if (e == "RG25") {
        bs = new BitSequenceBuilderRG(4);  //  25% of extra space for bitmaps -- cada 4*32=128 bits
    } else if (e == "RG33") {
        bs = new BitSequenceBuilderRG(3);  //33% of extra space for bitmaps -- cada 3*32=96 bits
    } else if (e == "RRR") {
        bs = new BitSequenceBuilderRRR(32);  // DEFAULT_SAMPLING for RRR is 32
    } else if (e == "SD") {
        bs = new BitSequenceBuilderSDArray();  // ?
    } else if (e == "SRRR15") {
        bs = new BitSequenceBuilder_SDSL_RRR_15();
    } else if (e == "SRRR31") {
        bs = new BitSequenceBuilder_SDSL_RRR_31();
    } else if (e == "SRRR63") {
        bs = new BitSequenceBuilder_SDSL_RRR_63();
    } else if (e == "SRRR127") {
        bs = new BitSequenceBuilder_SDSL_RRR_127();
    } else if (e == "SRRR255") {
        bs = new BitSequenceBuilder_SDSL_RRR_255();
    } else if (e == "SIL512") {
        bs = new BitSequenceBuilder_SDSL_IL_512(); //interleaved block size cada 512 -> 12.5%
    } else if (e == "SIL1024") {
        bs = new BitSequenceBuilder_SDSL_IL_1024(); //interleaved block size cada 1024 -> 6.25%
    } else if (e == "SIL128") {
        bs = new BitSequenceBuilder_SDSL_IL_128(); //interleaved block size cada 128 bits -> 0.5%
    }
    else {
        fprintf(stderr, "Error: bitmap '%s' not found.\n",e.c_str());
        return NULL;
    }

    return bs;
}

uint *getBitmap(BitSequence *bs) {
    size_t len = bs->getLength();

    uint *ret = new uint[len / W + 1];
    fill_n(ret, len / W + 1, 0); //fill with zeroes

    for(size_t i = 0; i < len; i++) {
        if (bs->access(i)==1) {
            cds_utils::bitset(ret,i);
        }
    }

    return ret;
}




  void UpdateMXCompactQtree::updateBitmaps(BitSequenceBuilder *bt) {
      uint *btemp;
      size_t len;

      for(int i = 0; i < depth_; i++ ) {
          fprintf(stderr, "New bitmaps at level %d\n",i);

          btemp = getBitmap(T_[i]);
          len = T_[i]->getLength();


          delete T_[i];
          T_[i] = bt->build(btemp, len);
          delete btemp;
      }
  }


  void UpdatePRBCompactQtree::updateBitmaps(BitSequenceBuilder *bt,BitSequenceBuilder *bb) {
      uint *btemp;
      size_t len;

      for(int i = 0; i < depth_; i++ ) {
          fprintf(stderr, "New bitmaps at level %d\n",i);

          btemp = getBitmap(T_[i]);
          len = T_[i]->getLength();


          delete T_[i];
          T_[i] = bt->build(btemp, len);
          delete btemp;



          btemp = getBitmap(B_[i]);
          len = B_[i]->getLength();

          delete B_[i];
          B_[i] = bb->build(btemp, len);
          delete btemp;
      }
  }



    void UpdatePRB2CompactQtree::updateBitmaps(BitSequenceBuilder *bt,BitSequenceBuilder *bb, BitSequenceBuilder *bc) {
        uint *btemp;
        size_t len;

        for(int i = 0; i < depth_; i++ ) {
            fprintf(stderr, "New bitmaps at level %d\n",i);

            btemp = getBitmap(T_[i]);
            len = T_[i]->getLength();


            delete T_[i];
            T_[i] = bt->build(btemp, len);
            delete btemp;



            btemp = getBitmap(B_[i]);
            len = B_[i]->getLength();

            delete B_[i];
            B_[i] = bb->build(btemp, len);
            delete btemp;

            btemp = getBitmap(C_[i]);
            len = C_[i]->getLength();

            delete C_[i];
            C_[i] = bc->build(btemp, len);
            delete btemp;
        }
    }
