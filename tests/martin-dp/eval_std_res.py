#! /usr/bin/python3

import sys
import eval_structure

RES_TRUE = "TRUE"
RES_UNKNOWN = "UNKNOWN"
RES_FALSE = "FALSE"

NAME_TRUE = "true"
NAME_FALSE = "false"

RESL = [RES_TRUE, RES_FALSE]
RESN = [NAME_TRUE, NAME_FALSE]
FALSEL = ["valid-memtrack", "valid-deref", "valid-free"]
FALSEU = "unreach"

def eval_res(name, res):
    if RES_UNKNOWN in res:
        return eval_structure.UNKNOWN
    elif NAME_FALSE in name:
        return eval_structure.REAL
    elif NAME_TRUE in name and RES_FALSE in res:
        return eval_structure.SPURIOUS
    else:
        return eval_structure.UNKNOWN
       
def eval_file(path):
    f = open(path)
    i = 0
    res = []
    act = {}

    for l in f:
        l = l.strip()
        if i == 0:
            act = {}
            act[eval_structure.PATH] = l
        elif i == 1:
            if RES_TRUE not in l and RES_FALSE not in l and RES_UNKNOWN not in l:
                act = {}
                act[eval_structure.PATH] = l
                i = 1
                continue
            error_type = eval_res(act[eval_structure.PATH], l)
            if error_type != eval_structure.UNKNOWN:
                act[eval_structure.TYPE] = error_type
                res.append(act)
        i = (i+1)%3

    return res
