/* This file is part of KCachegrind.
   Copyright (C) 2002, 2003 Josef Weidendorfer <Josef.Weidendorfer@gmx.de>

   KCachegrind is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/*
 * Classes holding profiling data for
 * multiple tracefiles for one command.
 * See class TraceData first.
 */

#ifndef TRACEDATA_H
#define TRACEDATA_H

#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qptrvector.h>
#include <qcolor.h>

#include "utils.h"

/**
 * All cost items are classes prefixed with "Trace".
 * "TraceCost" holds basic cost metrics for the simplest, smallest
 * trace entity: These are events counted for an instruction at
 * a specific memory address of the traced program.
 * All other cost items are derived from TraceCost, and add needed
 * cost metrics, e.g. for a call the number of calls that happened.
 *
 * Abstract, i.e. never instantiated cost items are
 * - TraceCost: Basic cost metrics (instr/read/write access + cache events)
 * - TraceCallCost: Additional call count cost metric.
 * - TraceCumulativeCost: Additional TraceCost aggregated.
 * - TraceListCost: Adds dependency to a list of TraceCost's
 * - TraceCallListCost: same for list of TraceCallCost's
 * - TraceCumulativeListCost: same for list of TraceCumulativeCost's
 * - TraceCostItem: Base for cost items for "interesting" costs:
 *              TraceFunction, TraceClass, TraceFile, TraceObject
 *
 * The smallest Cachegrind output is trace data indexed by a source
 * line number, a TracePartLine. Another one is a call from one
 * source line of a function to another function, a TracePartLineCall.
 * All other cost items derive the value by summation of cost metrics
 * from TraceLineItem and TracePartLineCall costs; their cost is
 * calculated lazy on demand and cached afterwards.
 *
 * For cost items, which are sums over all trace files read in, the
 * summed cost metrics change when e.g. a new trace file is read.
 * Thus, their cached costs are invalidated, and again recalculated
 * only on demand. In the following list, theses cost items are called
 * "dynamic", the other "fixed" (but neverless calculated lazy).
 *
 *  Cost Item          Type      Summation of ...
 *
 *  TracePartLineCall  fixed     Read from trace file
 *  TracePartLine      fixed     Read from trace file
 *  TracePartCall      fixed     TracePartLineCall's
 *  TraceLineCall      dynamic   TracePartLineCall's
 *  TraceCall          dynamic   TraceLineCall's
 *  TraceLine          dynamic   TracePartLine's and TraceLineCall's
 *  TracePartFunction  fixed     TracePartLine's / TracePartCall's
 *  TraceFunction      dynamic   TraceLine's / TraceCall's (called from)
 *  TracePartClass     fixed     TracePartFunction's
 *  TraceClass         dynamic   TraceFunction's
 *  TracePartFile      fixed     TracePartFunction's
 *  TraceFile          dynamic   TraceFunction's
 *  TracePartObject    fixed     TracePartFunction's
 *  TraceObject        dynamic   TraceFunction's
 *  TracePart          fixed     TracePartLine's
 *  TraceData          dynamic   TracePart's
 *
 * As there exists only one TraceData object for a traced program, its the
 * owner of some "high level" cost items. The following shows the owner
 * relationship of the cost item classes, together with references.
 *
 *  Cost Item          Owner (& back ref)   Other References to
 *
 *  TracePartLineCall  TraceLineCall
 *  TracePartCall      TraceCall            TracePartLineCall's
 *  TracePartLine      TraceLine            TracePartLineCall's
 *  TracePartFunction  TraceFunction
 *  TracePartClass     TraceClass           TracePart
 *  TracePartFile      TraceFile            TracePart
 *  TracePartObject    TraceObject          TracePart
 *  TraceLineCall      TraceCall            TracePartLineCall's
 *  TraceCall          TraceFunction        TracePartCall's
 *  TraceLine          TraceData            TraceLineCall's
 *  TraceFunction      TraceData            TraceCall's (calling)
 *  TraceClass         TraceData
 *  TraceFile          TraceData
 *  TraceObject        TraceData
 *  TracePart          TraceData
 *  TraceData          Main Application
 *
 * Convention:
 * - The owner has a factory method for owned objects,
 *   and calls addXXX() to install references in other objects
 * - The owner is first arg in a constructor.
 */


class FixString;
class FixCost;
class FixCallCost;
class FixJump;
class FixPool;
class TopLevel;

class TraceCost;
class TraceCostType;
class TraceCostMapping;
class TraceSubMapping;
class TraceJumpCost;
class TraceCallCost;
class TraceCumulativeCost;

class TracePartInstr;
class TracePartInstrCall;
class TracePartLine;
class TracePartLineCall;
class TracePartCall;
class TracePartFunction;
class TracePartClass;
class TracePartObject;
class TracePartFile;

class TraceInstr;
class TraceInstrJump;
class TraceInstrCall;
class TraceLine;
class TraceLineJump;
class TraceLineCall;
class TraceCall;
class TraceFunctionSource;
class TraceFunction;
class TraceFunctionCycle;
class TraceClass;
class TraceObject;
class TraceFile;
class TracePart;
class TraceData;

typedef QPtrList<TraceCost> TraceCostList;
typedef QPtrList<TraceJumpCost> TraceJumpCostList;
typedef QPtrList<TraceCallCost> TraceCallCostList;
typedef QPtrList<TraceCumulativeCost> TraceCumulativeCostList;

typedef QPtrList<TracePartCall>  TracePartCallList;
typedef QPtrList<TracePartInstr> TracePartInstrList;
typedef QPtrList<TracePartLine>  TracePartLineList;
typedef QPtrList<TracePartFunction>  TracePartFunctionList;
typedef QPtrList<TracePartInstrCall> TracePartInstrCallList;
typedef QPtrList<TracePartLineCall>  TracePartLineCallList;


typedef QPtrList<TraceInstr> TraceInstrList;
typedef QPtrList<TraceLine> TraceLineList;
typedef QPtrList<TraceInstrCall> TraceInstrCallList;
typedef QPtrList<TraceLineCall> TraceLineCallList;
typedef QPtrList<TraceCall> TraceCallList;
typedef QPtrList<TraceFile> TraceFileList;
typedef QPtrList<TraceFunctionSource> TraceFunctionSourceList;
typedef QPtrList<TraceFunction> TraceFunctionList;
typedef QPtrList<TraceFunctionCycle> TraceFunctionCycleList;
typedef QMap<QString, TraceObject> TraceObjectMap;
typedef QMap<QString, TraceClass> TraceClassMap;
typedef QMap<QString, TraceFile> TraceFileMap;
typedef QMap<QString, TraceFunction> TraceFunctionMap;
typedef QMap<uint, TraceLine> TraceLineMap;
typedef QMap<uint, TraceInstr> TraceInstrMap;


typedef unsigned long long uint64;

/**
 * Cost event counter, simple wrapper around a 64bit entity
 */
class SubCost
{
 public:
    SubCost() {}
    SubCost(uint64 i) { v=i; }
    SubCost(unsigned i) { v=i; }
    SubCost(int i) { v=(unsigned)i; }
    SubCost(double d) { v= (uint64)(d + .5); }

    SubCost& operator=(uint64 i) { v = i; return *this; }
    SubCost& operator=(unsigned i) { v = i; return *this; }
    SubCost& operator=(int i) { v = i; return *this; }
    SubCost& operator=(double d) { v = (uint64)(d + .5); return *this; }

    bool set(const char** s);
    bool set(FixString& s) { return s.stripUInt64(v); }

    operator uint64&() { return v; }

    bool operator==(unsigned i) const { return v == i; }
    bool operator==(int i) const { return v == (unsigned)i; }
    bool operator<(unsigned i) const { return v < i; }
    bool operator<(int i) const { return v < (unsigned)i; }
    bool operator<(const SubCost& s) const { return v < s.v; }
    bool operator>(unsigned i) const { return v > i; }
    bool operator>(int i) const { return v > (unsigned)i; }
    bool operator>(const SubCost& s) const { return v > s.v; }

    /**
     * Convert SubCost value into a QString,
     * spaced every 3 digits.
     */
    QString pretty();

    uint64 v;
};


/**
 * An abstract item from a trace file.
 */
class TraceItem
{
public:

  // RTTI for trace item classes, using type() method
  enum CostType { Item, Cost,
		  PartInstr, Instr,
                  PartLine, Line,
		  PartInstrJump, InstrJump,
                  PartLineJump, LineJump,
		  PartInstrCall, InstrCall,
                  PartLineCall, LineCall,
                  PartCall, Call,
                  PartFunction, FunctionSource, Function, FunctionCycle,
                  PartClass, Class, ClassCycle,
                  PartFile, File, FileCycle,
                  PartObject, Object, ObjectCycle,
                  Part, Data,
                  MaxCostType, NoCostType };

  TraceItem();
  virtual ~TraceItem();

  virtual CostType type() { return Item; }

  // conversion of item type to locale independent string (e.g. for config)
  static QString     typeName(CostType);
  static CostType    costType(QString);
  // the versions below should be used for user visible strings, as
  // these use localisation settings
  static QString     i18nTypeName(CostType);
  static CostType    i18nCostType(QString);

  /**
   * Returns dynamic name info (without type)
   */
  virtual QString name();

  /**
   * Same as name, but sometimes nicer for humans :-)
   */
  virtual QString prettyName();

  /**
   * Returns text of all cost metrics
   */
  virtual QString costString(TraceCostMapping*);

  /**
   * Returns type name + dynamic name
   */
  QString fullName();

  /**
   * Returns full name + cost text
   */
  QString toString();

  /**
   * Set all cost counters to zero
   */
  virtual void clear();

  /** Invalidate the cost attributes.
   * An invalidated object needs to be recalculated when a cost
   * attribute is requested (e.g. by subCost()).
   * Has to be overwritten by subclasses when the cost influences costs of
   * other cost items. If only one item depends on the cost of this item,
   * it can by set with setDependant() without a need for overwriting.
   */
  virtual void invalidate();

  /**
   * Sets a dependant to be invalidated when this cost is invalidated.
   * Call this function directly after the constructor.
   */
  void setDependant(TraceItem* d) { _dep = d; }

  TraceItem* dependant() { return _dep; }

  /**
   * If this item is not derived, but from a single trace file
   * set the trace file (=part) with this function.
   * Call this function directly after the constructor.
   */
  void setPart(TracePart* p) { _part = p; }

  TracePart* part() { return _part; }

protected:
  /** Updates cost attributes.
   * This has to be called by subclasses that access cost attributes
   * directly
   */
  virtual void update();

  bool _dirty;

  TracePart* _part;
  TraceItem* _dep;
};



/**
 * An array of basic cost metrics for a trace item.
 *
 * The semantic of specific indexes is stored in the
 * TraceCostMapping of the TraceData object holding this TraceCost.
 */
class TraceCost: public TraceItem
{
public:
  /**
   * The maximal number of subcosts a TraceCost can have.
   */
  static const int MaxRealIndex = 10;
  static const int InvalidIndex = -1;

  TraceCost();
  virtual ~TraceCost();

  virtual CostType type() { return Cost; }
  virtual QString costString(TraceCostMapping*);

  virtual void clear();

  // set the cost according to a submapping and a list of ASCII numbers
  void set(TraceSubMapping*, const char*);
  void set(TraceSubMapping*, FixString&);
  // add a cost according to a submapping and a list of ASCII numbers
  void addCost(TraceSubMapping*, const char*);
  void addCost(TraceSubMapping*, FixString&);
  // add the cost of another item
  void addCost(TraceCost* item);
  void addCost(int index, SubCost value);
  TraceCost diff(TraceCost* item);

  virtual void invalidate();

  /** Returns a sub cost. This automatically triggers
   * a call to update() if needed.
   */
  SubCost subCost(TraceCostType*);

  /**
   * Same as above, but only for real types
   */
  SubCost subCost(int);

  /** Returns a cost attribute converted to a string
   * (with space after 3 digits)
   */
  QString prettySubCost(TraceCostType*);

protected:
  virtual void update();

  SubCost _cost[MaxRealIndex];
  int _count; // only _count first indexes of _cost are used

  // cache last virtual subcost for faster access
  SubCost _cachedCost;
  TraceCostType* _cachedType;
};



/**
 * A cost type, e.g. "L1 Read Miss", short "l1rm".
 *
 * We distinguish "real" cost types, where the values come
 * from the trace file, and "virtual" cost types, which
 * are calculated from the real ones.
 *
 * For a virtual cost type, set a formula to calculate it:
 * e.g. for "Read Misses" : "l1rm + l2rm".
 * To allow for parsing, you must specify a TraceCostMapping
 * with according cost types (e.g. "l1rm" and "l2rm" for above formula).
 *
 * The cost type with empty name is the "const" cost type.
 */
class TraceCostType
{
public:

  /**
   * <name> is a short (non-localized) identifier for the cost type,
   *   e.g. "l1rm".
   * <longName> is a long localized string, e.g. "L1 Read Miss"
   * <formula> uses short names to reference other types
   */
  TraceCostType(QString name,
                QString longName = QString::null,
                QString formula = QString::null);

  void setName(QString n) { _name = n; }
  void setLongName(QString n) { _longName = n; }
  void setMapping(TraceCostMapping* m);
  void setFormula(QString);
  // default arg is for specifying a real type, but index unknown
  void setRealIndex(int r = TraceCost::MaxRealIndex);

  const QString& name() { return _name; }
  const QString& longName() { return _longName; }
  const QString& formula() { return _formula; }
  TraceCostMapping* mapping() { return _mapping; }
  int realIndex() { return _realIndex; }
  bool isReal() { return _formula.isEmpty(); }
  QColor color();

  /*
   * returns true if all cost type names can be resolved in formula
   */
  bool parseFormula();
  QString parsedFormula();

  SubCost subCost(TraceCost*);

  /*
   * For virtual costs, returns a histogram for use with
   * partitionPixmap().
   * Returns maximal real index.
   */
  int histCost(TraceCost* c, double total, double* hist);

  // application wide known types, referenced by short name
  static TraceCostType* knownRealType(QString);
  static TraceCostType* knownVirtualType(QString);
  static void add(TraceCostType*);
  static int knownTypeCount();
  static TraceCostType* knownType(int);

private:

  QString _name, _longName, _formula;
  TraceCostMapping* _mapping;
  bool _parsed, _inParsing;
  // index MaxRealIndex is for constant addition
  int _coefficient[TraceCost::MaxRealIndex];
  int _realIndex;

  static QPtrList<TraceCostType>* _knownTypes;
};


/**
 * A class for managing a set of cost types.
 *
 * Each cost type has an index:
 * - Real costs are in range [0 .. TraceCost:MaxRealIndex[
 * - Virtual costs are in range [MaxRealIndex, ...]
 */
class TraceCostMapping
{
public:
  TraceCostMapping();
  ~TraceCostMapping();

  /**
   * Defines a sub mapping with a list of real types
   * If <create> is false, checks if this is a existing sub mapping.
   */
  TraceSubMapping* subMapping(QString types, bool create = true);

  // "knows" about some real types
  int addReal(QString);
  int add(TraceCostType*);
  bool remove(TraceCostType*);
  int realCount() { return _realCount; }
  int virtualCount() { return _virtualCount; }
  int minVirtualIndex() { return TraceCost::MaxRealIndex; }
  TraceCostType* type(int);
  TraceCostType* realType(int);
  TraceCostType* virtualType(int);
  TraceCostType* type(QString);
  TraceCostType* typeForLong(QString);
  int realIndex(QString);
  int index(QString);
  QColor* realColors() { return _realColor; }

  /**
   * Adds all known virtual types that can be parsed
   */
  int addKnownVirtualTypes();

private:
  // we support only a fixed number of real and virtual types
  TraceCostType* _real[TraceCost::MaxRealIndex];
  QColor _realColor[TraceCost::MaxRealIndex];
  TraceCostType* _virtual[TraceCost::MaxRealIndex];
  int _realCount, _virtualCount;
};

/**
 * A submapping of a TraceCostMapping
 *
 * This is a fixed ordered list of indexes for real cost types
 * in a mapping.
 *
 * You can define a mapping by requesting submappings. Undefined cost
 * types will get a new real type index.
 *  TraceCostMapping m;
 *  sm1 = m.subMapping("Event1 Cost1 Cost2");  // returns submap [0,1,2]
 *  sm2 = m.subMapping("Event2 Cost3 Event1"); // returns submap [3,4,0]
 * Real types of m will be:
 *  (0:Event1, 1:Cost1, 2:Cost2, 3:Event2, 4:Cost3)
 */
class TraceSubMapping
{
public:
  TraceSubMapping(TraceCostMapping*);

  bool append(QString, bool create=true);
  bool append(int);
  void clear();

  /**
   * Get number of used indexes
   */
  int count() { return _count; }

  /**
   * Is this submapping the identity( i.e. realIndex(i)=i ) ?
   * This often allows for optimizations.
   */
  bool isIdentity() { return _isIdentity; }
  int realIndex(int i)
    { return (i<0 || i>=_count) ? TraceCost::InvalidIndex : _realIndex[i]; }

  /*
   * Allows an iteration over all real indexes not used in this submapping.
   * Usage: for(i = firstUnused();
   *            i != TraceCost::InvalidIndex;
   *            i = nextUnused(i)) { LOOP }
   */
  int firstUnused() { return _firstUnused; }
  int nextUnused(int i) {
      if (i<0 || i>=TraceCost::MaxRealIndex) return TraceCost::InvalidIndex;
      return _nextUnused[i]; }

private:
  TraceCostMapping* _mapping;
  int _count, _firstUnused;
  bool _isIdentity;
  int _realIndex[TraceCost::MaxRealIndex];
  int _nextUnused[TraceCost::MaxRealIndex];
};


/**
 * Cost of a (conditional) jump.
 */
class TraceJumpCost: public TraceItem
{
 public:
    TraceJumpCost();
    virtual ~TraceJumpCost();

    // reimplementations for cost addition
    virtual QString costString(TraceCostMapping* m);
    virtual void clear();

    void addCost(TraceJumpCost*);

    // additional cost metrics
    SubCost followedCount();
    SubCost executedCount();
    void addFollowedCount(SubCost c) { _followedCount += c; }
    void addExecutedCount(SubCost c) { _executedCount += c; }

protected:
    SubCost _executedCount, _followedCount;
};



/**
 * Cost item with additional call count metric.
 */
class TraceCallCost: public TraceCost
{
public:
  TraceCallCost();
  virtual ~TraceCallCost();

  // reimplementations for cost addition
  virtual QString costString(TraceCostMapping* m);
  virtual void clear();

  // additional cost metric
  SubCost callCount();
  QString prettyCallCount();
  void addCallCount(SubCost c);

protected:
  SubCost _callCount;
};


/**
 * Cost item with additional cumulative metric
 */
class TraceCumulativeCost: public TraceCost
{
public:
  TraceCumulativeCost();
  virtual ~TraceCumulativeCost();

  // reimplementations for cost addition
  virtual QString costString(TraceCostMapping* m);
  virtual void clear();

  // additional cost metric
  TraceCost* cumulative();
  void addCumulative(TraceCost*);

protected:
  TraceCost _cumulative;
};


/**
 * Cost Item
 * dependend on a list of cost items.
 */
class TraceListCost: public TraceCost
{
public:
  TraceListCost(bool onlyActiveParts = false);
  virtual ~TraceListCost();

  // reimplementation for dependency list
  virtual void update();

  TraceCostList& deps() { return _deps; }
  void addDep(TraceCost*);
  TraceCost* findDep(TracePart*);

protected:
  TraceCostList _deps;
  bool _onlyActiveParts;

private:
  // very temporary: cached
  TraceCost* _lastDep;
};


/**
 * Jump Cost Item
 * dependend on a list of Jump cost items.
 */
class TraceJumpListCost: public TraceJumpCost
{
public:
  TraceJumpListCost(bool onlyActiveParts = false);
  virtual ~TraceJumpListCost();

  // reimplementation for dependency list
  virtual void update();

  TraceJumpCostList deps() { return _deps; }
  void addDep(TraceJumpCost*);
  TraceJumpCost* findDep(TracePart*);

protected:

  TraceJumpCostList _deps;
  bool _onlyActiveParts;

private:
  // very temporary: cached
  TraceJumpCost* _lastDep;
};




/**
 * Call Cost Item
 * dependend on a list of Call cost items.
 */
class TraceCallListCost: public TraceCallCost
{
public:
  TraceCallListCost(bool onlyActiveParts = false);
  virtual ~TraceCallListCost();

  // reimplementation for dependency list
  virtual void update();

  TraceCallCostList deps() { return _deps; }
  void addDep(TraceCallCost*);
  TraceCallCost* findDep(TracePart*);

protected:

  TraceCallCostList _deps;
  bool _onlyActiveParts;

private:
  // very temporary: cached
  TraceCallCost* _lastDep;
};


/**
 * Cumulative Cost Item dependend on a list of cumulative cost items.
 */
class TraceCumulativeListCost: public TraceCumulativeCost
{
public:
  TraceCumulativeListCost(bool onlyActiveParts = false);
  virtual ~TraceCumulativeListCost();

  // reimplementation for dependency
  virtual void update();

  TraceCumulativeCostList deps() { return _deps; }
  void addDep(TraceCumulativeCost*);
  TraceCumulativeCost* findDep(TracePart*);

protected:
  TraceCumulativeCostList _deps;
  bool _onlyActiveParts;

private:
  // very temporary: cached
  TraceCumulativeCost* _lastDep;
};





/*-----------------------------------------------------------------
 * Classes for cost items of one trace file, i.e. a "trace part"
 *-----------------------------------------------------------------
 */

/**
 * Cost of jump at a instruction code address from a trace file.
 */
class TracePartInstrJump: public TraceJumpCost
{
 public:
    TracePartInstrJump(TraceInstrJump*, TracePart*);
    virtual ~TracePartInstrJump();

    virtual CostType type() { return PartInstrJump; }
    // fix cost item
    virtual void update() {}
    TraceInstrJump* instrJump() const { return (TraceInstrJump*) _dep; }
};


/**
 * Cost of a call at a instruction code address from a trace file.
 * Cost is always up to date, no lazy update needed.
 */
class TracePartInstrCall: public TraceCallCost
{
public:
  TracePartInstrCall(TraceInstrCall*, TracePart*);
  virtual ~TracePartInstrCall();

  virtual CostType type() { return PartInstrCall; }
  // fix cost item
  virtual void update() {}
  TraceInstrCall* instrCall() const { return (TraceInstrCall*) _dep; }
};


/**
 * Cost of a code instruction address from a trace file.
 * Cost is always up to date, no lazy update needed.
 */
class TracePartInstr: public TraceCost
{
public:
  TracePartInstr(TraceInstr*, TracePart*);
  virtual ~TracePartInstr();

  virtual CostType type() { return PartInstr; }
  // fix cost item
  virtual void update() {}

  TraceInstr* instr() const { return (TraceInstr*)_dep; }
};


/**
 * Cost of jump at a source line from a trace file.
 */
class TracePartLineJump: public TraceJumpCost
{
 public:
    TracePartLineJump(TraceLineJump*, TracePart*);
    virtual ~TracePartLineJump();

    virtual CostType type() { return PartLineJump; }
    // fix cost item
    virtual void update() {}
    TraceLineJump* lineJump() const { return (TraceLineJump*) _dep; }
};


/**
 * Cost of a call at a line from a trace file.
 * Cost is always up to date, no lazy update needed.
 */
class TracePartLineCall: public TraceCallCost
{
public:
  TracePartLineCall(TraceLineCall*, TracePart*);
  virtual ~TracePartLineCall();

  virtual CostType type() { return PartLineCall; }
  // fix cost item
  virtual void update() {}
  TraceLineCall* lineCall() const { return (TraceLineCall*) _dep; }
};



/**
 * Cost of a line from a trace file.
 * Cost is always up to date, no lazy update needed.
 */
class TracePartLine: public TraceCost
{
public:
  TracePartLine(TraceLine*, TracePart*);
  virtual ~TracePartLine();

  virtual CostType type() { return PartLine; }
  // fix cost item
  virtual void update() {}

  TraceLine* line() const { return (TraceLine*)_dep; }
};


/**
 * Cost of a call at a function to another function,
 * from a single trace file.
 */
class TracePartCall: public TraceCallListCost
{
public:
  TracePartCall(TraceCall* call, TracePart* part);
  virtual ~TracePartCall();

  virtual CostType type() { return PartCall; }
  // calls a function itself?
  bool isRecursion();

  // reimplementation for dependency list
  virtual void update();

  TraceCall* call() const { return (TraceCall*)_dep; }

  FixCallCost* setFirstFixCallCost(FixCallCost* fc)
    { FixCallCost* t = _firstFixCallCost; _firstFixCallCost = fc; return t; }
  FixCallCost* firstFixCallCost() const { return _firstFixCallCost; }

private:
  FixCallCost* _firstFixCallCost;
};


/**
 * Cost of a function,
 * from a single trace file.
 */
class TracePartFunction: public TraceCumulativeCost
{
public:
  TracePartFunction(TraceFunction*, TracePart*,
		    TracePartObject*, TracePartFile*);
  virtual ~TracePartFunction();

  virtual CostType type() { return PartFunction; }
  virtual void update();
  virtual QString costString(TraceCostMapping* m);

  void addPartInstr(TracePartInstr*);
  void addPartLine(TracePartLine*);
  void addPartCaller(TracePartCall*);
  void addPartCalling(TracePartCall*);

  TraceFunction* function() { return (TraceFunction*) _dep; }
  TracePartObject* partObject() { return _partObject; }
  TracePartClass* partClass() { return _partClass; }
  TracePartFile* partFile() { return _partFile; }
  const TracePartCallList& partCallers() { return _partCallers; }
  const TracePartCallList& partCallings() { return _partCallings; }
  void setPartObject(TracePartObject* o) { _partObject = o; }
  void setPartClass(TracePartClass* c) { _partClass = c; }
  void setPartFile(TracePartFile* f) { _partFile = f; }

  /* for linked list of FixXXX objects */
  FixCost* setFirstFixCost(FixCost* fc)
      { FixCost* t = _firstFixCost; _firstFixCost = fc; return t; }
  FixCost* firstFixCost() const { return _firstFixCost; }
  FixJump* setFirstFixJump(FixJump* fj)
      { FixJump* t = _firstFixJump; _firstFixJump = fj; return t; }
  FixJump* firstFixJump() const { return _firstFixJump; }

  // additional cost metrics
  SubCost calledCount();
  SubCost callingCount();
  QString prettyCalledCount();
  QString prettyCallingCount();
  int calledContexts();
  int callingContexts();

private:
  TracePartObject* _partObject;
  TracePartClass* _partClass;
  TracePartFile* _partFile;

  TracePartCallList _partCallings;
  TracePartCallList _partCallers;
  TracePartInstrList _partInstr;
  TracePartLineList _partLines;

  // cached
  SubCost _calledCount, _callingCount;
  int _calledContexts, _callingContexts;

  FixCost* _firstFixCost;
  FixJump* _firstFixJump;
};


/**
 * Cost of a class,
 * from a single trace file.
 */
class TracePartClass: public TraceCumulativeListCost
{
public:
  TracePartClass(TraceClass*, TracePart*);
  virtual ~TracePartClass();

  QString prettyName();
  virtual CostType type() { return PartClass; }
  TraceClass* cls() { return (TraceClass*)_dep; }
  void addPartFunction(TracePartFunction* f) { addDep(f); }
};


/**
 * Cost of a source file,
 * from a single trace file.
 */
class TracePartFile: public TraceCumulativeListCost
{
public:
  TracePartFile(TraceFile*, TracePart*);
  virtual ~TracePartFile();

  virtual CostType type() { return PartFile; }
  TraceFile* file() { return (TraceFile*)_dep; }
  void addPartFunction(TracePartFunction* f) { addDep(f); }
};


/**
 * Cost of a object,
 * from a single trace file.
 */
class TracePartObject: public TraceCumulativeListCost
{
public:
  TracePartObject(TraceObject*, TracePart*);
  virtual ~TracePartObject();

  virtual CostType type() { return PartObject; }
  TraceObject* object() { return (TraceObject*)_dep; }
  void addPartFunction(TracePartFunction* f) { addDep(f); }
};



/**
 * A Trace Part: All data read from a trace file, containing all costs
 * that happened in a specified time interval of the executed command.
 */
class TracePart: public TraceListCost
{
public:
  TracePart(TraceData*, QString file);
  virtual ~TracePart();

  virtual CostType type() { return Part; }

  QString shortName() const;
  QString prettyName();
  QString name() { return _name; }
  QString description() const { return _descr; }
  QString trigger() const { return _trigger; }
  QString timeframe() const { return _timeframe; }
  QString version() const { return _version; }
  int partNumber() { return _number; }
  int threadID() { return _tid; }
  int processID() { return _pid; }
  void setDescription(const QString& d) { _descr = d; }
  void setTrigger(const QString& t) { _trigger = t; }
  void setTimeframe(const QString& t) { _timeframe = t; }
  void setVersion(const QString& v) { _version = v; }
  void setPartNumber(int n);
  void setThreadID(int t);
  void setProcessID(int p);
  //const TraceCostList& items() const { return _items; }
  TraceData* data() const { return _data; }
  TraceCost* totals() { return &_totals; }
  void setFixSubMapping(TraceSubMapping* sm) { _fixSubMapping = sm; }
  TraceSubMapping* fixSubMapping() { return _fixSubMapping; }

  // returns true if something changed
  bool activate(bool);
  bool isActive() { return _active; }

private:
  QString _name;

  QString _descr;
  QString _trigger;
  QString _timeframe;
  QString _version;

  int _number, _tid, _pid;
  //TraceCostList _items;
  TraceData* _data;

  bool _active;

  // the totals line
  TraceCost _totals;

  // submapping for all fix costs of this part
  TraceSubMapping* _fixSubMapping;
};


class TracePartList: public QPtrList<TracePart>
{
protected:
  int compareItems ( Item item1, Item item2 );
};


/*-----------------------------------------------------------------
 * Classes for cost items summed up from multiple trace parts
 *-----------------------------------------------------------------
 */


/**
 * A jump from an instruction to another inside of a function
 */
class TraceInstrJump: public TraceJumpListCost
{
public:
    TraceInstrJump(TraceInstr* instrFrom, TraceInstr* instrTo,
		   bool isCondJump);
    virtual ~TraceInstrJump();

    virtual CostType type() { return InstrJump; }
    virtual QString name();

    TraceInstr* instrFrom() const { return _instrFrom; }
    TraceInstr* instrTo() const { return _instrTo; }
    bool isCondJump() { return _isCondJump; }

    // part factory
    TracePartInstrJump* partInstrJump(TracePart*);

 private:
    TraceInstr *_instrFrom, *_instrTo;
    bool _isCondJump;
};

class TraceInstrJumpList: public QPtrList<TraceInstrJump>
{
 public:
    TraceInstrJumpList() { _sortLow = true; }
    void setSortLow(bool s) { _sortLow = s; }

protected:
  int compareItems ( Item item1, Item item2 );

 private:
  bool _sortLow;
};


/**
 * A jump from one line to another inside of a function.
 */
class TraceLineJump: public TraceJumpListCost
{
public:
  TraceLineJump(TraceLine* lineFrom, TraceLine* lineTo,
		bool isCondJump);
  virtual ~TraceLineJump();

  virtual CostType type() { return LineJump; }
  virtual QString name();

  TraceLine* lineFrom() const { return _lineFrom; }
  TraceLine* lineTo() const { return _lineTo; }
  bool isCondJump() { return _isCondJump; }

  // part factory
  TracePartLineJump* partLineJump(TracePart*);

private:
  TraceLine *_lineFrom, *_lineTo;
  bool _isCondJump;
};


class TraceLineJumpList: public QPtrList<TraceLineJump>
{
 public:
    TraceLineJumpList() { _sortLow = true; }
    void setSortLow(bool s) { _sortLow = s; }

protected:
  int compareItems ( Item item1, Item item2 );

 private:
  bool _sortLow;
};


/**
 * A call from an instruction of one function to another function
 */
class TraceInstrCall: public TraceCallListCost
{
public:
  TraceInstrCall(TraceCall* call, TraceInstr* instr);
  virtual ~TraceInstrCall();

  virtual CostType type() { return InstrCall; }
  virtual QString name();

  TraceInstr* instr() const { return _instr; }
  TraceCall* call() const { return _call; }

  // part factory
  TracePartInstrCall* partInstrCall(TracePart*, TracePartCall*);
private:
  TraceInstr* _instr;
  TraceCall* _call;
};


/**
 * A call from a line of one function to another function.
 */
class TraceLineCall: public TraceCallListCost
{
public:
  TraceLineCall(TraceCall* call, TraceLine* line);
  virtual ~TraceLineCall();

  virtual CostType type() { return LineCall; }
  virtual QString name();

  TraceLine* line() const { return _line; }
  TraceCall* call() const { return _call; }

  // part factory
  TracePartLineCall* partLineCall(TracePart*, TracePartCall*);
private:
  TraceLine* _line;
  TraceCall* _call;
};


/**
 * A call from one to another function.
 * Consists of a list a TraceLineCalls
 */
class TraceCall: public TraceCallListCost
{
public:
  TraceCall(TraceFunction* caller, TraceFunction* called);
  virtual ~TraceCall();

  virtual CostType type() { return Call; }
  virtual QString name();

  // calls a function itself?
  bool isRecursion() { return _caller == _called; }

  // return cycle number >0 if call is inside of a cycle
  int inCycle();
  // we need some special handling for cycle calls
  void update();

  void invalidateDynamicCost();

  // factories
  TracePartCall* partCall(TracePart*,
                          TracePartFunction*, TracePartFunction*);
  TraceLineCall* lineCall(TraceLine*);
  TraceInstrCall* instrCall(TraceInstr*);

  TraceFunction* caller(bool skipCycle=false) const;
  TraceFunction* called(bool skipCycle=false) const;
  QString callerName(bool skipCycle=false) const;
  QString calledName(bool skipCycle=false) const;
  const TraceLineCallList& lineCalls() const { return _lineCalls; }
  const TraceInstrCallList& instrCalls() const { return _instrCalls; }

  FixCallCost* setFirstFixCost(FixCallCost* fc)
    { FixCallCost* t = _firstFixCost; _firstFixCost = fc; return t; }

private:
  TraceInstrCallList _instrCalls;
  TraceLineCallList _lineCalls;
  TraceFunction* _caller;
  TraceFunction* _called;

  FixCallCost* _firstFixCost;
};


/**
 * A code instruction address of the program.
 * Consists of a list a TracePartInstr from different trace files
 * and a list of TraceInstrCalls if there are calls from this address.
 */
class TraceInstr: public TraceListCost
{
public:
  TraceInstr();
  virtual ~TraceInstr();

  virtual CostType type() { return Instr; }
  virtual QString name();
  QString prettyName();

  bool isValid() { return _addr != 0; }

  // factories
  TracePartInstr* partInstr(TracePart* part,
			    TracePartFunction* partFunction);
  TraceInstrJump* instrJump(TraceInstr* to, bool isCondJump);

  void addInstrCall(TraceInstrCall*);

  uint addr() const { return _addr; }
  TraceFunction* function() const { return _function; }
  TraceLine* line() const { return _line; }
  const TraceInstrJumpList& instrJumps() const { return _instrJumps; }
  const TraceInstrCallList& instrCalls() const { return _instrCalls; }
  bool hasCost(TraceCostType*);

  // only to be called after default constructor
  void setAddr(uint addr) { _addr = addr; }
  void setFunction(TraceFunction* f) { _function = f; }
  void setLine(TraceLine* l) { _line = l; }

private:
  uint _addr;
  TraceFunction* _function;
  TraceLine* _line;

  TracePartInstrList _items;
  TraceInstrJumpList _instrJumps;
  TraceInstrCallList _instrCalls;
};


/**
 * A source line of the program.
 * Consists of a list a TracePartLines from different trace files
 * and a list of TraceLineCalls if there are calls from this line.
 */
class TraceLine: public TraceListCost
{
public:
  TraceLine();
  virtual ~TraceLine();

  virtual CostType type() { return Line; }
  virtual QString name();
  QString prettyName();

  // factories
  TracePartLine* partLine(TracePart* part,
                          TracePartFunction* partFunction);
  TraceLineJump* lineJump(TraceLine* to, bool isCondJump);

  void addLineCall(TraceLineCall*);


  bool isValid() { return _sourceFile != 0; }
  bool hasCost(TraceCostType*);
  TraceFunctionSource* functionSource() const { return _sourceFile; }
  uint lineno() const { return _lineno; }
  uint items() const { return _items.count(); }
  const TraceLineCallList& lineCalls() const { return _lineCalls; }
  const TraceLineJumpList& lineJumps() const { return _lineJumps; }

  // only to be called after default constructor
  void setSourceFile(TraceFunctionSource* sf) { _sourceFile = sf; }
  void setLineno(uint lineno) { _lineno = lineno; }

private:
  TraceFunctionSource* _sourceFile;
  uint _lineno;

  TracePartLineList _items;
  TraceLineJumpList _lineJumps;
  TraceLineCallList _lineCalls;
};


/*
 * Base class for all costs which
 * represent "interesting" items or group of items
 * with settable name and cumulative cost
 */
class TraceCostItem: public TraceCumulativeListCost
{
public:
  TraceCostItem();
  virtual ~TraceCostItem();

  virtual QString name() { return _name; }
  TraceData* data() { return _data; }
  void setData(TraceData* data) { _data = data; }
  virtual void setName(const QString& name) { _name = name; }

protected:
  TraceData* _data;
  QString _name;
};



/**
 * A container helper class for TraceFunction for source lines
 * where a function is implemented in.
 * With inlining, lines of the same function can come from
 * different source files.
 * A instance of this class holds all lines of one source file
 * in a map
 */
class TraceFunctionSource: public TraceCost
{
public:
  TraceFunctionSource(TraceFunction*, TraceFile*);
  virtual ~TraceFunctionSource();

  virtual QString name();
  virtual CostType type() { return FunctionSource; }

  // reimplementation for dependency map
  virtual void update();

  TraceFile* file() { return _file; }
  TraceFunction* function() { return _function; }
  uint firstLineno();
  uint lastLineno();
  TraceLineMap* lineMap();

  void invalidateDynamicCost();

  /* factory */
  TraceLine* line(uint lineno, bool createNew = true);

private:
  TraceFile* _file;
  TraceFunction* _function;
  TraceLineMap* _lineMap;
  TraceLine* _line0;

  bool _lineMapFilled;
};


/**
 * For temporary assoziation of objects with TraceFunctions.
 * Used in coverage analysis and TreeMap drawing.
 */
class TraceAssoziation
{
public:
  /**
   * Creates an invalid assoziation.
   */
  TraceAssoziation();
  virtual ~TraceAssoziation();

  // for runtime detection
  virtual int rtti() { return 0; }

  /**
   * Could we set the function assoziation to ourself?
   * This only can return false if this is a unique assoziation.
   */
  bool isAssoziated();

  /**
   * reset function to assoziate this object to.
   * returns true if assoziation could be established
   */
  bool setFunction(TraceFunction*);
  TraceFunction* function() { return _function; }

  void invalidate() { _valid = false; }
  bool isValid() { return _valid; }

  /**
   * Delete all assoziations in TraceFunctions of data with
   * rtti runtime info. rtti = 0: delete ALL assoziations.
   */
  static void clear(TraceData* data, int rtti);

  /**
   * Invalidate all assoziations in TraceFunctions of data with
   * rtti runtime info. rtti = 0: Invalidate ALL assoziations.
   */
  static void invalidate(TraceData* data, int rtti);

protected:
  TraceFunction* _function;
  bool _valid;
};

typedef QPtrList<TraceAssoziation> TraceAssoziationList;

/**
 * A traced function
 *
 * References to functions are stored in
 *  (1) a function map in TraceData (by value)
 *  (2) a TraceClass
 */
class TraceFunction: public TraceCostItem
{
public:
  TraceFunction();
  TraceFunction(TraceData* data, const QString& name,
                TraceClass* cls, TraceFile* file, TraceObject* object);
  virtual ~TraceFunction();

  virtual CostType type() { return Function; }
  virtual void update();

  // this invalidate all subcosts of function depending on
  // active status of parts
  void invalidateDynamicCost();

  void addCaller(TraceCall*);

  // factories
  TraceCall* calling(TraceFunction* called);
  TraceLine* line(TraceFile*, uint lineno, bool createNew = true);
  TraceInstr* instr(uint addr, bool createNew = true);
  TracePartFunction* partFunction(TracePart*,
                                  TracePartFile*, TracePartObject*);

  QString prettyName();
  QString location() const;
  QString info(); // prettyName + location
  TraceClass* cls() const { return _cls; }
  TraceFile* file() const { return _file; }
  TraceObject* object() const { return _object; }
  // get the source file with lines from function declaration (not inlined)
  TraceFunctionSource* sourceFile(TraceFile* file = 0,
                                      bool createNew = false);
  const TraceFunctionSourceList& sourceFiles() const
    { return _sourceFiles; }
  TraceCallList callers(bool skipCycle=false) const;
  const TraceCallList& callings(bool skipCycle=false) const;

  uint firstAddress() const;
  uint lastAddress() const;
  TraceInstrMap* instrMap();

  // cost metrics
  SubCost calledCount();
  SubCost callingCount();
  QString prettyCalledCount();
  QString prettyCallingCount();
  int calledContexts();
  int callingContexts();

  // only to be called after default constructor
  void setFile(TraceFile* file) { _file = file; }
  void setObject(TraceObject* object) { _object = object; }
  void setClass(TraceClass* cls) { _cls = cls; }
  void setMapIterator(TraceFunctionMap::Iterator it) { _myMapIterator = it; }

  // see TraceFunctionAssoziation
  void addAssoziation(TraceAssoziation* a);
  void removeAssoziation(TraceAssoziation* a);
  void removeAssoziation(int rtti, bool reallyDelete = true);
  void invalidateAssoziation(int rtti);
  TraceAssoziation* assoziation(int rtti);

  // cycles
  void setCycle(TraceFunctionCycle* c) { _cycle = c; }
  TraceFunctionCycle* cycle() { return _cycle; }
  bool isCycle();
  bool isCycleMember();
  void cycleReset();
  void cycleDFS(int d, int& pNo, TraceFunction** pTop);

protected:
  TraceCallList _callers; // list of calls we are called from
  TraceCallList _callings; // list of calls we are calling (we are owner)
  TraceFunctionCycle* _cycle;

private:
  bool isUniquePrefix(QString);
  TraceFunctionMap::Iterator _myMapIterator;

  TraceClass* _cls;
  TraceObject* _object;
  TraceFile* _file;

  TraceFunctionSourceList _sourceFiles; // we are owner
  TraceInstrMap* _instrMap; // we are owner
  bool _instrMapFilled;

  // see TraceAssoziation
  TraceAssoziationList _assoziations;

  // for cycle detection
  int _cycleLow;
  TraceFunction* _cycleStackDown;

  // cached
  SubCost _calledCount, _callingCount;
  int _calledContexts, _callingContexts;
};


/**
 * A cycle of recursive calling functions.
 *
 * This is itself shown as a function
 */
class TraceFunctionCycle: public TraceFunction
{
public:
  TraceFunctionCycle(TraceFunction*, int n);

  virtual CostType type() { return FunctionCycle; }

  // this removes all members from this cycle
  void init();
  void add(TraceFunction*);
  // this sets up the cycle once members are added
  void setup();

  TraceFunction* base() { return _base; }
  int cycleNo() { return _cycleNo; }
  const TraceFunctionList& members() { return _members; }

private:
  TraceFunction* _base;
  int _cycleNo;

  TraceFunctionList _members;
};


/**
 * A C++ Class / Namespace
 *
 * If a function symbol has a prefix ending in "::",
 * the prefix is supposed to be a class/namespace specifier.
 * Without such a prefix, we put a symbol in the "(global)" namespace.
 */
class TraceClass: public TraceCostItem
{
public:
  TraceClass();
  virtual ~TraceClass();

  virtual QString prettyName();
  virtual CostType type() { return Class; }
  void addFunction(TraceFunction*);
  const TraceFunctionList& functions() const { return _functions; }

  // part factory
  TracePartClass* partClass(TracePart*);

private:
  TraceFunctionList _functions;
};



/**
 * A source file containing function definitions
 */
class TraceFile: public TraceCostItem
{
public:
  TraceFile();
  virtual ~TraceFile();

  virtual CostType type() { return File; }
  void setDirectory(const QString& dir);
  void resetDirectory() { _dir = QString::null; }
  QString directory();

  void addFunction(TraceFunction*);
  void addSourceFile(TraceFunctionSource*);

  // without path
  QString shortName() const;
  QString prettyName() { return shortName(); }
  QString prettyLongName();
  const TraceFunctionList& functions() const { return _functions; }
  const TraceFunctionSourceList& sourceFiles() const
    { return _sourceFiles; }

  // part factory
  TracePartFile* partFile(TracePart*);

private:
  TraceFunctionList _functions;
  TraceFunctionSourceList _sourceFiles;
  QString _dir;
};


/**
 * A object containing a text segment (shared lib/executable)
 * with defined functions
 */
class TraceObject: public TraceCostItem
{
public:
  TraceObject();
  virtual ~TraceObject();

  virtual CostType type() { return Object; }

  void addFunction(TraceFunction*);

  virtual void setName(const QString& name);
  QString shortName() const { return _shortName; }
  QString prettyName() { return shortName(); }
  const TraceFunctionList& functions() const { return _functions; }

  // part factory
  TracePartObject* partObject(TracePart*);

private:
  TraceFunctionList _functions;
  QString _shortName;
};



/**
 * This class holds profiling data of multiple tracefiles
 * generated with cachegrind on one command.
 *
 */
class TraceData: public TraceCost
{
public:
  TraceData(TopLevel* top = 0);
  TraceData(const QString& base);
  virtual ~TraceData();

  virtual CostType type() { return Data; }

  /**
   * Loads a trace file.
   *
   * This adjusts the TraceCostMapping according to given cost types
   */
  void load(const QString&);

  static QString tracePrefix();

  /** returns true if something changed. These do NOT
   * invalidate the dynamic costs on a activation change,
   * i.e. all cost items dependend on active parts.
   * This has to be done by the caller when true is returned by
   * calling invalidateDynamicCost().
   */
  bool activateParts(const TracePartList&);
  bool activateParts(TracePartList, bool active);
  bool activatePart(TracePart*, bool active);
  bool activateAll(bool active=true);

  TracePartList parts() const { return _parts; }
  TracePart* part(QString& name);

  // with path
  QString traceName() { return _traceName; }

  // without path
  QString shortTraceName();
  QString activePartRange();

  TraceCostMapping* mapping() { return &_mapping; }

  // memory pool for FixCost instances
  FixPool* fixPool();

  // factories for object/file/class/function/line instances
  TraceObject* object(const QString& name);
  TraceFile* file(const QString& name);
  TraceClass* cls(const QString& fnName, QString& shortName);
  // function creation involves class creation if needed
  TraceFunction* function(const QString& name, TraceFile*, TraceObject*);
  // factory for function cycles
  TraceFunctionCycle* functionCycle(TraceFunction*);

  /** 
   * Search for item with given name and highest subcost of given cost type.
   *
   * For some items, they will only be found if the parent cost is given:
   *  Instr, Line, Call  => need parent of type Function 
   * For Function, a parent of type Obj/File/Class can be given, but
   * isn't needed.
   */
  TraceCost* search(TraceItem::CostType, QString,
		    TraceCostType* ct = 0, TraceCost* parent = 0);

  // same factories with support for compressed format
  TraceObject* compressedObject(const QString& name);
  TraceFile* compressedFile(const QString& name);
  TraceFunction* compressedFunction(const QString& name,
                                    TraceFile*, TraceObject*);

  // for pretty function names without signature if unique...
  TraceFunctionMap::Iterator functionIterator(TraceFunction*);
  TraceFunctionMap::Iterator functionBeginIterator();
  TraceFunctionMap::Iterator functionEndIterator();

  TraceObjectMap& objectMap() { return _objectMap; }
  TraceFileMap& fileMap() { return _fileMap; }
  TraceClassMap& classMap() { return _classMap; }
  TraceFunctionMap& functionMap() { return _functionMap; }

  const TraceFunctionCycleList& functionCycles() { return _functionCycles; }

  void setCommand(const QString& command) { _command = command; }
  QString command() const { return _command; }
  TraceCost* totals() { return &_totals; }
  void setMaxThreadID(int tid) { _maxThreadID = tid; }
  int maxThreadID() { return _maxThreadID; }
  void setMaxPartNumber(int n) { _maxPartNumber = n; }
  int maxPartNumber() { return _maxPartNumber; }

  // reset all manually set directories for source files
  void resetSourceDirs();

  virtual void update();

  // invalidates all cost items dependant on active state of parts
  void invalidateDynamicCost();

  // cycle detection
  void updateFunctionCycles();
  void updateObjectCycles();
  void updateClassCycles();
  void updateFileCycles();
  bool inFunctionCycleUpdate() { return _inFunctionCycleUpdate; }

private:
  void init();
  // add trace part: events from one trace file
  TracePart* addPart(const QString& dir, const QString& file);

  // for progress bar callbacks
  TopLevel* _topLevel;

  TracePartList _parts;

  // The mapping for all costs
  TraceCostMapping _mapping;

  FixPool* _fixPool;

  // always the trace totals (not dependent on active parts)
  TraceCost _totals;
  int _maxThreadID;
  int _maxPartNumber;

  TraceObjectMap _objectMap;
  TraceClassMap _classMap;
  TraceFileMap _fileMap;
  TraceFunctionMap _functionMap;
  QString _command;
  QString _traceName;

  // support for compressed format
  QPtrVector<TraceObject> _objectVector;
  QPtrVector<TraceFile> _fileVector;
  QPtrVector<TraceFunction> _functionVector;

  // cycles
  TraceFunctionCycleList _functionCycles;
  int _functionCycleCount;
  bool _inFunctionCycleUpdate;
};



#endif