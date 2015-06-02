/*
 * sdsl_bitvectors.h
 *
 *  Created on: Nov 4, 2014
 *      Author: diegocaro
 */

#ifndef SDSL_BITVECTORS_H_
#define SDSL_BITVECTORS_H_

#include <sdsl/bit_vectors.hpp>
#include <libcdsBasics.h>
#include <BitSequenceBuilder.h>

using namespace sdsl;
using namespace cds_utils;
using namespace cds_static;

#define SDSL_RRR_15_HDR 1001
#define SDSL_RRR_31_HDR 1002
#define SDSL_RRR_63_HDR 1003
#define SDSL_RRR_255_HDR 1004
#define SDSL_RRR_127_HDR 1005

#define SDSL_IL_512_HDR 1101
#define SDSL_IL_1024_HDR 1102
#define SDSL_IL_128_HDR 1103

// to avoid the unused parameter warning... ugly, but it maintains
// the requirements of the super class
#ifndef UNUSED
	#define UNUSED(x) (void)(x)
#endif

namespace cqtree_utils {


/*
class BitSequence {
public:
    static BitSequence * load(ifstream & fp);

    virtual bool access(size_t i) = 0;

    virtual size_t rank0(const size_t i) = 0;
    virtual size_t select0(const size_t i) = 0;
    virtual size_t rank1(const size_t i) = 0;
    virtual size_t select1(const size_t i) = 0;
};*/



template<uint16_t block_size = 15>
class _SDSL_RRR {
public:
    _SDSL_RRR() {}

    _SDSL_RRR(const bit_vector& bv) {
        _rrr = rrr_vector<block_size>(bv);
        _build_rank_select();
    }

    _SDSL_RRR(const uint * bitseq, size_t len) {
        // Please, optmize this copy!! @TODO
        bit_vector bv(len);
        for(size_t i = 0; i < len; i++) {
            if (bitget(bitseq,i)==1) bv[i] = 1;
        }
        _rrr = rrr_vector<block_size>(bv);
        _build_rank_select();
    }

    ~_SDSL_RRR() {
    }

    void _build_rank_select() {
        assert(&_rrr != NULL);
        _rank1_rrr = typename rrr_vector<block_size>::rank_1_type(&_rrr);
        _rank0_rrr = typename rrr_vector<block_size>::rank_0_type(&_rrr);

        _select1_rrr = typename rrr_vector<block_size>::select_1_type(&_rrr);
        _select0_rrr = typename rrr_vector<block_size>::select_0_type(&_rrr);
    }

    bool access(size_t i) { return _rrr[i];}
    size_t rank1(size_t i) { return _rank1_rrr(i+1); }
    size_t rank0(size_t i) { return _rank0_rrr(i+1); }
    size_t select1(size_t i) { return _select1_rrr(i); }
    size_t select0(size_t i) { return _select0_rrr(i); }

    void save_stream(ofstream &f) const {
        _rrr.serialize(f);
    }

    static _SDSL_RRR<block_size> * load_stream(ifstream &f) {
        _SDSL_RRR<block_size> *ret = new _SDSL_RRR<block_size>;
        ret->_rrr.load(f);
        ret->_build_rank_select();
        return ret;
    }

    size_t getLength() {
        return _rrr.size();
    }

    size_t getSize() {
        return size_in_bytes(_rrr);
    }

protected:
    rrr_vector<block_size> _rrr;
    typename rrr_vector<block_size>::rank_1_type _rank1_rrr;
    typename rrr_vector<block_size>::rank_0_type _rank0_rrr;
    typename rrr_vector<block_size>::select_1_type _select1_rrr;
    typename rrr_vector<block_size>::select_0_type _select0_rrr;
};

class SDSL_RRR_15 : public BitSequence {
public:
    SDSL_RRR_15(): bv(NULL) {}

    SDSL_RRR_15(const uint * bitseq, size_t len): bv(NULL) {
        bv = new _SDSL_RRR<15>(bitseq,len);

        this->length = len;
        this->ones = rank1(len-1);
    }
    ~SDSL_RRR_15() {
        if (bv != NULL) delete bv;
    }

    static SDSL_RRR_15* load(ifstream &f) {
        uint type = cds_utils::loadValue<uint>(f);
        if(type!=SDSL_RRR_15_HDR) {
            abort();
        }
        SDSL_RRR_15 *ret = new SDSL_RRR_15();
        ret->bv = _SDSL_RRR<15>::load_stream(f);
        ret->length = ret->bv->getLength();
        ret->ones = ret->bv->rank1(ret->length-1);
        return ret;
    }

    void save(ofstream &f) const {
        uint wr = SDSL_RRR_15_HDR;
        saveValue(f,wr);
        bv->save_stream(f);
    }

    virtual size_t getSize() const {return bv->getSize();}

    virtual bool access(const size_t i) const { return bv->access(i);}
    virtual size_t rank1(const size_t i) const { return bv->rank1(i); }
    virtual size_t rank0(const size_t i) const { return bv->rank0(i); }
    virtual size_t select1(const size_t i) const { return bv->select1(i); }
    virtual size_t select0(const size_t i) const { return bv->select0(i); }

private:
    _SDSL_RRR<15> *bv;
};

class SDSL_RRR_31 : public BitSequence {
public:
    SDSL_RRR_31(): bv(NULL) {}

    SDSL_RRR_31(const uint * bitseq, size_t len): bv(NULL) {
        bv = new _SDSL_RRR<31>(bitseq,len);

        this->length = len;
        this->ones = rank1(len-1);
    }
    ~SDSL_RRR_31() {
        if (bv != NULL) delete bv;
    }

    static SDSL_RRR_31* load(ifstream &f) {
        uint type = cds_utils::loadValue<uint>(f);
        if(type!=SDSL_RRR_31_HDR) {
            abort();
        }
        SDSL_RRR_31 *ret = new SDSL_RRR_31();
        ret->bv = _SDSL_RRR<31>::load_stream(f);
        ret->length = ret->bv->getLength();
        ret->ones = ret->bv->rank1(ret->length-1);
        return ret;
    }

    void save(ofstream &f) const {
        uint wr = SDSL_RRR_31_HDR;
        saveValue(f,wr);
        bv->save_stream(f);
    }

    virtual size_t getSize() const {return bv->getSize();}

    virtual bool access(const size_t i) const { return bv->access(i);}
    virtual size_t rank1(const size_t i) const { return bv->rank1(i); }
    virtual size_t rank0(const size_t i) const { return bv->rank0(i); }
    virtual size_t select1(const size_t i) const { return bv->select1(i); }
    virtual size_t select0(const size_t i) const { return bv->select0(i); }

private:
    _SDSL_RRR<31> *bv;
};


class SDSL_RRR_63 : public BitSequence {
public:
    SDSL_RRR_63(): bv(NULL) {}

    SDSL_RRR_63(const uint * bitseq, size_t len): bv(NULL) {
        bv = new _SDSL_RRR<63>(bitseq,len);

        this->length = len;
        this->ones = rank1(len-1);
    }
    ~SDSL_RRR_63() {
        if (bv != NULL) delete bv;
    }

    static SDSL_RRR_63* load(ifstream &f) {
        uint type = cds_utils::loadValue<uint>(f);
        if(type!=SDSL_RRR_63_HDR) {
            abort();
        }
        SDSL_RRR_63 *ret = new SDSL_RRR_63();
        ret->bv = _SDSL_RRR<63>::load_stream(f);
        ret->length = ret->bv->getLength();
        ret->ones = ret->bv->rank1(ret->length-1);
        return ret;
    }

    void save(ofstream &f) const {
        uint wr = SDSL_RRR_63_HDR;
        saveValue(f,wr);
        bv->save_stream(f);
    }

    virtual size_t getSize() const {return bv->getSize();}

    virtual bool access(const size_t i) const { return bv->access(i);}
    virtual size_t rank1(const size_t i) const { return bv->rank1(i); }
    virtual size_t rank0(const size_t i) const { return bv->rank0(i); }
    virtual size_t select1(const size_t i) const { return bv->select1(i); }
    virtual size_t select0(const size_t i) const { return bv->select0(i); }

private:
    _SDSL_RRR<63> *bv;
};

class SDSL_RRR_127 : public BitSequence {
public:
    SDSL_RRR_127(): bv(NULL) {}

    SDSL_RRR_127(const uint * bitseq, size_t len): bv(NULL) {
        bv = new _SDSL_RRR<127>(bitseq,len);

        this->length = len;
        this->ones = rank1(len-1);
    }
    ~SDSL_RRR_127() {
        if (bv != NULL) delete bv;
    }

    static SDSL_RRR_127* load(ifstream &f) {
        uint type = cds_utils::loadValue<uint>(f);
        if(type!=SDSL_RRR_127_HDR) {
            abort();
        }
        SDSL_RRR_127 *ret = new SDSL_RRR_127();
        ret->bv = _SDSL_RRR<127>::load_stream(f);
        ret->length = ret->bv->getLength();
        ret->ones = ret->bv->rank1(ret->length-1);
        return ret;
    }

    void save(ofstream &f) const {
        uint wr = SDSL_RRR_127_HDR;
        saveValue(f,wr);
        bv->save_stream(f);
    }

    virtual size_t getSize() const {return bv->getSize();}

    virtual bool access(const size_t i) const { return bv->access(i);}
    virtual size_t rank1(const size_t i) const { return bv->rank1(i); }
    virtual size_t rank0(const size_t i) const { return bv->rank0(i); }
    virtual size_t select1(const size_t i) const { return bv->select1(i); }
    virtual size_t select0(const size_t i) const { return bv->select0(i); }

private:
    _SDSL_RRR<127> *bv;
};


class SDSL_RRR_255 : public BitSequence {
public:
    SDSL_RRR_255(): bv(NULL) {}

    SDSL_RRR_255(const uint * bitseq, size_t len): bv(NULL) {
        bv = new _SDSL_RRR<255>(bitseq,len);

        this->length = len;
        this->ones = rank1(len-1);
    }
    ~SDSL_RRR_255() {
        if (bv != NULL) delete bv;
    }

    static SDSL_RRR_255* load(ifstream &f) {
        uint type = cds_utils::loadValue<uint>(f);
        if(type!=SDSL_RRR_255_HDR) {
            abort();
        }
        SDSL_RRR_255 *ret = new SDSL_RRR_255();
        ret->bv = _SDSL_RRR<255>::load_stream(f);
        ret->length = ret->bv->getLength();
        ret->ones = ret->bv->rank1(ret->length-1);
        return ret;
    }

    void save(ofstream &f) const {
        uint wr = SDSL_RRR_255_HDR;
        saveValue(f,wr);
        bv->save_stream(f);
    }

    virtual size_t getSize() const {return bv->getSize();}

    virtual bool access(const size_t i) const { return bv->access(i);}
    virtual size_t rank1(const size_t i) const { return bv->rank1(i); }
    virtual size_t rank0(const size_t i) const { return bv->rank0(i); }
    virtual size_t select1(const size_t i) const { return bv->select1(i); }
    virtual size_t select0(const size_t i) const { return bv->select0(i); }

private:
    _SDSL_RRR<255> *bv;
};


template<class BV>
class _SDSL_BV {
public:
    _SDSL_BV() {}

    _SDSL_BV(const bit_vector& bv) {
        _bv = BV(bv);
        _build_rank_select();
    }

    _SDSL_BV(const uint * bitseq, size_t len) {
        // Please, optmize this copy!! @TODO
        bit_vector bv(len);
        for(size_t i = 0; i < len; i++) {
            if (bitget(bitseq,i)==1) bv[i] = 1;
        }
        _bv = BV(bv);
        _build_rank_select();
    }

    ~_SDSL_BV() {
    }

    void _build_rank_select() {
        assert(&_bv != NULL);
        _rank1_bv = typename BV::rank_1_type(&_bv);
        _rank0_bv = typename BV::rank_0_type(&_bv);

        _select1_bv = typename BV::select_1_type(&_bv);
        _select0_bv = typename BV::select_0_type(&_bv);
    }

    bool access(size_t i) { return _bv[i];}
    size_t rank1(size_t i) { return _rank1_bv(i+1); }
    size_t rank0(size_t i) { return _rank0_bv(i+1); }
    size_t select1(size_t i) { return _select1_bv(i); }
    size_t select0(size_t i) { return _select0_bv(i); }

    void save_stream(ofstream &f) const {
        _bv.serialize(f);
    }

    static _SDSL_BV<BV> * load_stream(ifstream &f) {
        _SDSL_BV<BV> *ret = new _SDSL_BV<BV>;
        ret->_bv.load(f);
        ret->_build_rank_select();
        return ret;
    }

    size_t getLength() {
        return _bv.size();
    }

    size_t getSize() {
        return size_in_bytes(_bv);
    }

protected:
    BV _bv;
    typename BV::rank_1_type _rank1_bv;
    typename BV::rank_0_type _rank0_bv;
    typename BV::select_1_type _select1_bv;
    typename BV::select_0_type _select0_bv;

};


class SDSL_IL_512 : public BitSequence {
public:
    SDSL_IL_512(): bv(NULL) {}

    SDSL_IL_512(const uint * bitseq, size_t len): bv(NULL) {
        bv = new _SDSL_BV< bit_vector_il<512> >(bitseq,len);

        this->length = len;
        this->ones = rank1(len-1);
    }
    ~SDSL_IL_512() {
        if (bv != NULL) delete bv;
    }

    static SDSL_IL_512* load(ifstream &f) {
        uint type = cds_utils::loadValue<uint>(f);
        if(type!=SDSL_IL_512_HDR) {
            abort();
        }
        SDSL_IL_512 *ret = new SDSL_IL_512();
        ret->bv = _SDSL_BV< bit_vector_il<512> >::load_stream(f);
        ret->length = ret->bv->getLength();
        ret->ones = ret->bv->rank1(ret->length-1);
        return ret;
    }

    void save(ofstream &f) const {
        uint wr = SDSL_IL_512_HDR;
        saveValue(f,wr);
        bv->save_stream(f);
    }

    virtual size_t getSize() const {return bv->getSize();}

    virtual bool access(const size_t i) const { return bv->access(i);}
    virtual size_t rank1(const size_t i) const { return bv->rank1(i); }
    virtual size_t rank0(const size_t i) const { return bv->rank0(i); }
    virtual size_t select1(const size_t i) const { return bv->select1(i); }
    virtual size_t select0(const size_t i) const { return bv->select0(i); }

private:
    _SDSL_BV< bit_vector_il<512> > *bv;
};


class SDSL_IL_1024 : public BitSequence {
public:
    SDSL_IL_1024(): bv(NULL) {}

    SDSL_IL_1024(const uint * bitseq, size_t len): bv(NULL) {
        bv = new _SDSL_BV< bit_vector_il<1024> >(bitseq,len);

        this->length = len;
        this->ones = rank1(len-1);
    }
    ~SDSL_IL_1024() {
        if (bv != NULL) delete bv;
    }

    static SDSL_IL_1024* load(ifstream &f) {
        uint type = cds_utils::loadValue<uint>(f);
        if(type!=SDSL_IL_1024_HDR) {
            abort();
        }
        SDSL_IL_1024 *ret = new SDSL_IL_1024();
        ret->bv = _SDSL_BV< bit_vector_il<1024> >::load_stream(f);
        ret->length = ret->bv->getLength();
        ret->ones = ret->bv->rank1(ret->length-1);
        return ret;
    }

    void save(ofstream &f) const {
        uint wr = SDSL_IL_1024_HDR;
        saveValue(f,wr);
        bv->save_stream(f);
    }

    virtual size_t getSize() const {return bv->getSize();}

    virtual bool access(const size_t i) const { return bv->access(i);}
    virtual size_t rank1(const size_t i) const { return bv->rank1(i); }
    virtual size_t rank0(const size_t i) const { return bv->rank0(i); }
    virtual size_t select1(const size_t i) const { return bv->select1(i); }
    virtual size_t select0(const size_t i) const { return bv->select0(i); }

private:
    _SDSL_BV< bit_vector_il<1024> > *bv;
};


class SDSL_IL_128 : public BitSequence {
public:
    SDSL_IL_128(): bv(NULL) {}

    SDSL_IL_128(const uint * bitseq, size_t len): bv(NULL) {
        bv = new _SDSL_BV< bit_vector_il<128> >(bitseq,len);

        this->length = len;
        this->ones = rank1(len-1);
    }
    ~SDSL_IL_128() {
        if (bv != NULL) delete bv;
    }

    static SDSL_IL_128* load(ifstream &f) {
        uint type = cds_utils::loadValue<uint>(f);
        if(type!=SDSL_IL_128_HDR) {
            abort();
        }
        SDSL_IL_128 *ret = new SDSL_IL_128();
        ret->bv = _SDSL_BV< bit_vector_il<128> >::load_stream(f);
        ret->length = ret->bv->getLength();
        ret->ones = ret->bv->rank1(ret->length-1);
        return ret;
    }

    void save(ofstream &f) const {
        uint wr = SDSL_IL_128_HDR;
        saveValue(f,wr);
        bv->save_stream(f);
    }

    virtual size_t getSize() const {return bv->getSize();}

    virtual bool access(const size_t i) const { return bv->access(i);}
    virtual size_t rank1(const size_t i) const { return bv->rank1(i); }
    virtual size_t rank0(const size_t i) const { return bv->rank0(i); }
    virtual size_t select1(const size_t i) const { return bv->select1(i); }
    virtual size_t select0(const size_t i) const { return bv->select0(i); }

private:
    _SDSL_BV< bit_vector_il<128> > *bv;
};

class NewBitSequence:public BitSequence {
public:
    static BitSequence * load(ifstream & fp) {
        uint r = cds_utils::loadValue<uint>(fp);
        size_t pos = fp.tellg();
        fp.seekg(pos-sizeof(uint));
        switch(r) {
            case RRR02_HDR: return BitSequenceRRR::load(fp);
            case BRW32_HDR: return BitSequenceRG::load(fp);
            //case RRR02_LIGHT_HDR: return BitSequence_rrr02_light::load(fp);
            case SDARRAY_HDR: return BitSequenceSDArray::load(fp);
            case DARRAY_HDR: return BitSequenceDArray::load(fp);

            case SDSL_RRR_15_HDR: return SDSL_RRR_15::load(fp);
            case SDSL_RRR_31_HDR: return SDSL_RRR_31::load(fp);
            case SDSL_RRR_63_HDR: return SDSL_RRR_63::load(fp);
            case SDSL_RRR_127_HDR: return SDSL_RRR_127::load(fp);
            case SDSL_RRR_255_HDR: return SDSL_RRR_255::load(fp);
            case SDSL_IL_512_HDR: return SDSL_IL_512::load(fp);
            case SDSL_IL_1024_HDR: return SDSL_IL_1024::load(fp);
            case SDSL_IL_128_HDR: return SDSL_IL_128::load(fp);
        }
        return NULL;
    }
};

/*
BitSequence* BitSequence::load(ifstream &fp) {
    uint r = loadValue<uint>(fp);
    size_t pos = fp.tellg();
    fp.seekg(pos-sizeof(uint));
    switch(r) {
        case SDSL_RRR_15_HDR: return SDSL_RRR_15::load(fp);
        case SDSL_RRR_31_HDR: return SDSL_RRR_31::load(fp);
        case SDSL_RRR_63_HDR: return SDSL_RRR_63::load(fp);
        case SDSL_RRR_255_HDR: return SDSL_RRR_255::load(fp);
    }
    return NULL;
}
*/

class BitSequenceBuilder_SDSL_RRR_15 : public BitSequenceBuilder {
    public:
        BitSequenceBuilder_SDSL_RRR_15() {};
        virtual ~BitSequenceBuilder_SDSL_RRR_15() {}
        virtual BitSequence * build(uint * bitseq, size_t len) const { return new SDSL_RRR_15(bitseq,len); };
        virtual BitSequence * build(const BitString & bs) const { UNUSED(bs); return NULL;};
};

class BitSequenceBuilder_SDSL_RRR_31 : public BitSequenceBuilder {
    public:
        BitSequenceBuilder_SDSL_RRR_31() {};
        virtual ~BitSequenceBuilder_SDSL_RRR_31() {}
        virtual BitSequence * build(uint * bitseq, size_t len) const { return new SDSL_RRR_31(bitseq,len); };
        virtual BitSequence * build(const BitString & bs) const { UNUSED(bs); return NULL;};
};

class BitSequenceBuilder_SDSL_RRR_63 : public BitSequenceBuilder {
    public:
        BitSequenceBuilder_SDSL_RRR_63() {};
        virtual ~BitSequenceBuilder_SDSL_RRR_63() {}
        virtual BitSequence * build(uint * bitseq, size_t len) const { return new SDSL_RRR_63(bitseq,len); };
        virtual BitSequence * build(const BitString & bs) const { UNUSED(bs); return NULL;};
};

class BitSequenceBuilder_SDSL_RRR_127 : public BitSequenceBuilder {
    public:
        BitSequenceBuilder_SDSL_RRR_127() {};
        virtual ~BitSequenceBuilder_SDSL_RRR_127() {}
        virtual BitSequence * build(uint * bitseq, size_t len) const { return new SDSL_RRR_127(bitseq,len); };
        virtual BitSequence * build(const BitString & bs) const { UNUSED(bs); return NULL;};
};

class BitSequenceBuilder_SDSL_RRR_255 : public BitSequenceBuilder {
    public:
        BitSequenceBuilder_SDSL_RRR_255() {};
        virtual ~BitSequenceBuilder_SDSL_RRR_255() {}
        virtual BitSequence * build(uint * bitseq, size_t len) const { return new SDSL_RRR_255(bitseq,len); };
        virtual BitSequence * build(const BitString & bs) const { UNUSED(bs); return NULL;};
};

class BitSequenceBuilder_SDSL_IL_512 : public BitSequenceBuilder {
    public:
        BitSequenceBuilder_SDSL_IL_512() {};
        virtual ~BitSequenceBuilder_SDSL_IL_512() {}
        virtual BitSequence * build(uint * bitseq, size_t len) const { return new SDSL_IL_512(bitseq,len); };
        virtual BitSequence * build(const BitString & bs) const { UNUSED(bs); return NULL;};
};

class BitSequenceBuilder_SDSL_IL_1024 : public BitSequenceBuilder {
    public:
        BitSequenceBuilder_SDSL_IL_1024() {};
        virtual ~BitSequenceBuilder_SDSL_IL_1024() {}
        virtual BitSequence * build(uint * bitseq, size_t len) const { return new SDSL_IL_1024(bitseq,len); };
        virtual BitSequence * build(const BitString & bs) const { UNUSED(bs); return NULL;};
};

class BitSequenceBuilder_SDSL_IL_128 : public BitSequenceBuilder {
    public:
        BitSequenceBuilder_SDSL_IL_128() {};
        virtual ~BitSequenceBuilder_SDSL_IL_128() {}
        virtual BitSequence * build(uint * bitseq, size_t len) const { return new SDSL_IL_128(bitseq,len); };
        virtual BitSequence * build(const BitString & bs) const { UNUSED(bs); return NULL;};
};


}

#endif /* SDSL_BITVECTORS_H_ */
