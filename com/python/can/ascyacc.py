'''
/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2015  AS <parai@foxmail.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
'''

from ply import *
import asclex
import sys

tokens = asclex.tokens

precedence = (
               ('left', 'PLUS','MINUS'),
               ('left', 'TIMES','DIVIDE'),
               ('left', 'POWER')
)

def p_objectList(p):
    '''objectList : objectList object 
                    | object '''
    if(len(p)==2):
        if(not p[0]):p[0] = {}
        p[0].update(p[1])
    elif(len(p)==3):
        p[0] = p[1]
        if(not p[0]):p[0] = {}
        p[0].update(p[2])

def p_object(p):
    '''object :  version
                | ns 
                | bs 
                | bu
                | boList
                | evList'''
    if(not p[0]): p[0] = {}
    p[0].update(p[1])
    
def p_version(p):
    '''version : VERSION STRING END'''
    p[0] = {'version':p[2]}

def p_ns(p):
    '''ns : NS_ COLON EOL nsList END'''
    p[0] = {'ns':p[4]}
    
def p_bs(p):
    '''bs : BS_ COLON idList END'''
    p[0] = {'bs':p[3]}  
    
def p_bu(p):
    '''bu : BU_ COLON idList END'''
    p[0] = {'bu':p[3]} 

def p_boList(p):
    ''' boList : boList bo 
            | bo '''
    if(len(p)==2):
        if(not p[0]):p[0] = {'boList':[]}
        p[0]['boList'].append(p[1])
    elif(len(p)==3):
        p[0] = p[1]
        if(not p[0]):p[0] = {'boList':[]}
        p[0]['boList'].append(p[2])
def p_bo(p):
    '''bo : BO_ INTEGER ID COLON INTEGER ID EOL sgList END'''
    p[0] = {'bo':{'id':p[2],'name':p[3],'length':p[5],'node':p[6],'sgList':p[8]}}  

def p_evList(p):
    ''' evList : evList ev 
        | ev '''
    if(len(p)==2):
        if(not p[0]):p[0] = {'evList':[]}
        p[0]['evList'].append(p[1])
    elif(len(p)==3):
        p[0] = p[1]
        if(not p[0]):p[0] = {'evList':[]}
        p[0]['evList'].append(p[2])      
def p_ev(p):
    '''ev : EV_ ID COLON INTEGER LBK INTEGER  OR INTEGER RBK STRING INTEGER INTEGER idList SEMI END'''
    p[0] = p[1]

def p_sgList(p):
    '''sgList : sgList sg
                | sg''' 
    if(len(p)==2):
        if(not p[0]):p[0] = {'sgList':[]}
        p[0]['sgList'].append(p[1])
    elif(len(p)==3):
        p[0] = p[1]
        if(not p[0]):p[0] = {'sgList':[]}
        p[0]['sgList'].append(p[2])

def p_sg(p):
    '''sg : SG_ ID COLON INTEGER OR INTEGER AT INTEGER PLUS LPAREN INTEGER COMMA INTEGER RPAREN LBK INTEGER OR INTEGER RBK STRING idList EOL'''
    p[0] = {'sg':{'name':p[2],'start':p[4],'size':p[6],'u':p[8],'factor':p[11],'offset':p[13],'min':p[16],'max':p[18],'unit':p[20],'node':p[21]}}

def p_idList(p):
    '''idList : idList ID
                    | ID
                    | empty '''
    if(len(p)==2):
        if(not p[0]):p[0] = {'idList':[]}
        p[0]['idList'].append(p[1])
    elif(len(p)==3):
        p[0] = p[1]
        if(not p[0]):p[0] = {'idList':[]}
        p[0]['idList'].append(p[2])
        
def p_nsList(p):
    '''nsList : nsList ID EOL
                    | ID EOL '''
    if(len(p)==3):
        if(not p[0]):p[0] = {'nsList':[]}
        p[0]['nsList'].append(p[1])
    elif(len(p)==4):
        p[0] = p[1]
        if(not p[0]):p[0] = {'nsList':[]}
        p[0]['nsList'].append(p[2])        
        
def p_END(p):
    ''' END : END EOL
               | EOL'''
    if(len(p)==3):
        p[0] = p[2]
    else:
        p[0] = p[1]
     
def p_empty(p):
    '''empty :'''

#### Catastrophic error handler
def p_error(p):
    if not p:
        print("SYNTAX ERROR AT EOF")
    else:
        try:
            print("SYNTAX ERROR AT LINE(%s) %s"%(p.lineno,p))
        except:
            print("SYNTAX ERROR AT LINE(%s) %s"%(int(p),p))
    while 1:
        tok = yacc.token()             # Get the next token
        if not tok or tok.type == 'EOL': break
    yacc.errok()

    return tok  

bparser = yacc.yacc()

def parse(data,debug=0):
    bparser.error = 0
    p = bparser.parse(data,debug=debug)
    if bparser.error: return None
    return p  