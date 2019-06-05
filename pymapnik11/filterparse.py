#-----------------------------------------------------------------------
#
# This file is part of pymapnik11
#
# Copyright (C) 2018 James Harris
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
#-----------------------------------------------------------------------

from __future__ import unicode_literals, print_function
import itertools, collections

from arpeggio import *
from arpeggio import RegExMatch as _


class IKwd(StrMatch):
    """
    A specialization of StrMatch to specify keywords of the language.
    """
    def __init__(self, to_match):
        super(IKwd, self).__init__(to_match,ignore_case=True)
        self.to_match = to_match
        self.root = True
        self.rule_name = 'keyword'

def true():         return IKwd("true")
def number():       return _(r'\-?(\d+\.\d*|\d+)')
def strliteral():   return _(r'\'(?:[^\']|\'\')*\'')
def label():        return "[",_(r'[a-z|A-Z][a-z|A-Z|0-9|_|\-|/|:]*'),"]"
def null():         return IKwd("null")
def cmp_operator():     return map(StrMatch, ["!=","<=", ">=","<",">","=","~="])
def field_operator(): return map(StrMatch, ["*","-","+","/"])

def field():        return [number, strliteral, label, null, field_operation, true]
def field_operation(): return "(", field, field_operator, field, ")"
def operation_clause(): return "(", field, cmp_operator, field, ")"
def and_clause():   return "(", clause, IKwd("and"), clause, ")"
def or_clause():   return "(", clause, IKwd("or"), clause, ")"
def dotmatch_clause():  return field, IKwd(".match"), "(", field, ")"

def clause():       return [operation_clause, and_clause, or_clause, dotmatch_clause]

def mapnikfilter(): return [field, clause]



class And:
    def __init__(self, l, r):
        self.l, self.r = l, r
    def __call__(self, feat):
        return self.l(feat) and self.r(feat)
        
    def find_key(self, key):
        ans=self.l.find_key(key)
        if ans:
            return ans
        return self.r.find_key(key)

    @property
    def js(self):
        return "style_and(%s, %s)" % (self.l.js, self.r.js)

    
    @property
    def json(self):
        return {'name':'And', 'Left': self.l.json, 'Right': self.r.json}

    @property
    def keys(self):
        for k in self.l.keys: yield k
        for k in self.r.keys: yield k

    def __str__(self):
        return "And(%s,%s)" % (self.l,self.r)

class Or:
    def __init__(self, l, r):
        self.l, self.r = l, r
    def __call__(self, feat):
        return self.l(feat) or self.r(feat)
        
    def find_key(self, key):
        ans=self.l.find_key(key)
        if ans:
            return ans
        return self.r.find_key(key)

    @property
    def keys(self):
        for k in self.l.keys: yield k
        for k in self.r.keys: yield k

    @property
    def js(self):
        return "style_or(%s, %s)" % (self.l.js, self.r.js)

    def __str__(self):
        return "Or(%s,%s)" % (self.l,self.r)
    
    @property
    def json(self):
        return {'name':'Or', 'Left': self.l.json, 'Right': self.r.json}
    
class Op:
    def __init__(self, l, o, r):
        self.l,self.o,self.r = l,o,r
    def __call__(self, feat):
        l,r = self.l(feat),self.r(feat)
        if type(l)!=list: l=[l]
        if type(r)!=list: r=[r]
        for li in l:
            for ri in r:
                if self.test(li,ri):
                    return True
        return False

    def find_key(self, key):
        if isinstance(self.l,Label) and isinstance(self.r,Value):
            if self.l.key==key:
                return self.r.val
        return None


    def test(self,l,r):

        if self.o == '=':
            return l == r
        elif self.o == '!=':
            return l != r

        if l==None or r==None:
            return False

        if self.o == '>':
            return l > r
        elif self.o == '>=':
            return l >= r
        elif self.o == '<':
            return l < r
        elif self.o == '<=':
            return l <= r
        elif self.o == '~=':
            return re.match(r,l)
        print("??",l, self.o, r)
        return False
    def __str__(self):
        return "Op(%s, %s, %s)" % (self.l,self.o,self.r)

    @property
    def keys(self):
        for k in self.l.keys: yield k
        for k in self.r.keys: yield k

    @property
    def js(self):
        assert self.o in ('=','!=','>','<','<=','>=','~=')
        return "style_op(%s, '%s', %s)" % (self.l.js, self.o, self.r.js)


    @property
    def json(self):
        return {'name':'Op', 'Left': self.l.json, 'Op': self.o, 'Right': self.r.json}

class FieldOp:
    def __init__(self, l, o, r):
        self.l,self.o,self.r = l,o,r
    def __call__(self, feat):
        l,r = self.l(feat),self.r(feat)
        if type(l)!=list: l=[l]
        if type(r)!=list: r=[r]
        for li in l:
            for ri in r:
                if self.test(li,ri):
                    return True
        return False

    def find_key(self, key):
        if isinstance(self.l,Label) and isinstance(self.r,Value):
            if self.l.key==key:
                return self.r.val
        return None


    def test(self,l,r):

        
        if self.o == '+':
            return l + r
        elif self.o == '-':
            return l - r
        elif self.o == '*':
            return l * r
        elif self.o == '/':
            return l / r
        
        print("??",l, self.o, r)
        return False
    def __str__(self):
        return "FieldOp(%s, %s, %s)" % (self.l,self.o,self.r)

    @property
    def keys(self):
        for k in self.l.keys: yield k
        for k in self.r.keys: yield k

    @property
    def js(self):
        assert self.o in ('+','-','/','*')
        return "style_fieldop(%s, '%s', %s)" % (self.l.js, self.o, self.r.js)
    
    @property
    def json(self):
        return {'name':'ColumnOp', 'Left': self.l.json, 'Right': self.r.json, 'Op': self.o}

class Label:
    def __init__(self, key):
        self.key=key
    def __str__(self):
        return "[%s]" % str(self.key)
    def __call__(self, feat):
        if self.key in feat['properties']:
            return feat['properties'][self.key]
        return None
    @property
    def js(self):
        return "style_label(%s)" % repr(self.key)

    @property
    def keys(self):
        return [self.key]
    
    def find_key(self, key):
        return None
    
    
    @property
    def json(self):
        return {'name':'Label', 'Key': self.key}
    

class Value:
    def __init__(self, val):
        self.val = val
    def __str__(self):
        return '"%s"' % (repr(self.val),)
    def __call__(self, feat):
        return self.val

    @property
    def keys(self):
        return []

    @property
    def js(self):
        if self.val==None:
            return "style_value(null)"
        return "style_value(%s)" % repr(self.val)

    def find_key(self, key):
        return None
    
    @property
    def json(self):
        return {'name':'Value', 'Value': self.val}

class Visitor(PTNodeVisitor):
    def visit_number(self,n,c):
        try:
            return Value(int(str(n)))
        except:
            return Value(float(str(n)))
    def visit_strliteral(self,n,c):
        r=unicode(c[0] if c else n)
            
        if r[0]=="'" and r[-1]=="'":
            return Value(r[1:-1])
        return Value(r)
    def visit_true(self, n, c):
        return Value(True)
    
    def visit_null(self,n,c):
        return Value(None)

    def visit_label(self,n,c):
        return Label(c[0])

    def visit_field(self, n, c):
        return c[0]
    def visit_field_operation(self, n, c):
        return FieldOp(c[0],c[1],c[2])
    def visit_operation_clause(self, n, c):
        return Op(c[0], c[1], c[2])

    def visit_and_clause(self, n, c):
        return And(c[0], c[1])

    def visit_or_clause(self, n, c):
        return Or(c[0], c[1])

    def visit_dotmatch_clause(self, n, c):
        return Op(c[0], '~=', c[1])

    def visit_clause(self, n, c):
        return c[0]

    def visit_mapnikfilter(self, n, c):
        return c[0]

parser = ParserPython(mapnikfilter)
proc = lambda qu: visit_parse_tree(parser.parse(qu), Visitor())
