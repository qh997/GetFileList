#!/usr/bin/python
# -*- coding: utf-8 -*

import argparse
import os
import time
import sys
import stat

VERSION = '1.0'

def init():
    parser = argparse.ArgumentParser(
        description = 'Find and print file list by specified path(s).',)

    parser.add_argument(
        '-t', '--type',
        choices = ['reg', 'dir', 'sym'],
        action = 'append',
        help = 'Specify the file type of the searching.(default: reg)')
    parser.add_argument(
        '-d', '--depth',
        type = int,
        default = -1,
        help = 'Specify the depth of the searching.(default: %(default)s)')
    parser.add_argument(
        '-o', '--order',
        choices = ['pos', 'rev', 'nul'],
        default = 'pos',
        help = 'Specify the order of output.(default: %(default)s)')
    parser.add_argument(
        '-s', '--sort-by',
        choices = ['path', 'file', 'size', 'date'],
        default = 'path',
        help = 'Specify the key of sort.(default: %(default)s)')
    parser.add_argument(
        '-p', '--output',
        choices = ['abs', 'rel', 'bsn'],
        default = 'rel',
        help = 'Specify the style of output.(default: %(default)s)')
    parser.add_argument(
        '-g', '--group',
        action = 'store_true',
        default = False,
        help = 'Frouping the result.(default: %(default)s)')
    parser.add_argument(
        '--debug',
        type = int,
        default = 0,
        metavar = 'DEBUG-LEVEL',
        help = 'Set the debug level.(default: %(default)s)')
    parser.add_argument(
        '-v', '--version',
        action = 'version',
        version = '%(prog)s ' + VERSION,
        default = argparse.SUPPRESS,
        help = 'Display the version of this program.')

    parser.add_argument(
        'paths',
        nargs = '*',
        default = [os.getcwd() + os.path.sep],
        help = 'The path list.(default: current dir)')

    args = parser.parse_args()
    global FTYPE, SRCHDEP, ORD, SORTBY, OUTPUT, GROUP, DEBUG
    FTYPE = args.type if args.type else ['reg']
    SRCHDEP = args.depth
    ORD = args.order
    SORTBY = args.sort_by
    OUTPUT = args.output
    GROUP = args.group
    DEBUG = args.debug

    return [p if p[-1] == os.path.sep else p + os.path.sep for p in args.paths]

def debug(level, *msg):
    if level <= DEBUG:
        ori_time = time.time();
        crt_time = (time.strftime('%H:%M:%S',time.localtime(ori_time))
                 + '.' + str(ori_time - int(ori_time))[2:5])
        pmsg = ''
        for m in msg:
            pmsg += str(m)
        print "[DebugMessage %s %d] %s" % (crt_time, level, pmsg)

def getFileList(path, depth, ftype):
    dirlist = []

    if depth:
        if os.path.exists(path):
            for name in os.listdir(path):
                fullname = os.path.join(path, name)
                st = os.lstat(fullname)
                isappend = False

                if stat.S_ISDIR(st.st_mode):
                    dirlist += getFileList(fullname, depth - 1, ftype)
                    if 'dir' in ftype:
                        isappend = True
                elif 'reg' in ftype and stat.S_ISREG(st.st_mode):
                    isappend = True
                elif 'sym' in ftype and stat.S_ISREG(st.st_mode):
                    isappend = True

                if isappend: dirlist.append([fullname, st.st_size, st.st_mtime])
        else:
            sys.stderr.write('[ERROR] Can not found path: {}\n'.format(path))

    return dirlist

if __name__ == '__main__':
    dirpaths = init()
    debug(2, 'FTYPE = ', FTYPE)
    debug(2, 'SRCHDEP = ', SRCHDEP)
    debug(2, 'ORD = ', ORD)
    debug(2, 'SORTBY = ', SORTBY)
    debug(2, 'OUTPUT = ', OUTPUT)
    debug(2, 'GROUP = ', GROUP)
    debug(2, 'DEBUG = ', DEBUG)
    debug(2, 'dirpath = ', dirpaths)

    filelist = []
    for dirpath in sorted(dirpaths):
        debug(1, dirpath)
        ori_list = getFileList(dirpath, SRCHDEP, FTYPE)
        debug(1, len(ori_list), ' files.')
        filelist.append([dirpath, ori_list])

    frist_g = True
    for listgroup in filelist:
        debug(1, listgroup[0])

        if GROUP:
            if not frist_g: print
            print listgroup[0]
            frist_g  = False

        sorted_files = []

        if ORD != 'null':
            rev = True if ORD == 'reverse' else False
            if SORTBY == 'pathname':
                sorted_files = sorted(listgroup[1], key = lambda x: x[0], reverse = rev)
            elif SORTBY == 'filename':
                sorted_files = sorted(listgroup[1], key = lambda x: os.path.basename(x[0]), reverse = rev)
            elif SORTBY == 'filesize':
                sorted_files = sorted(listgroup[1], key = lambda x: x[1], reverse = rev)
            elif SORTBY == 'filedate':
                sorted_files = sorted(listgroup[1], key = lambda x: x[2], reverse = rev)
        else:
            sorted_files = listgroup[1]

        for sfile in sorted_files:
            debug(3, sfile)
            if OUTPUT == 'absolute':
                print sfile[0]
            elif OUTPUT == 'relative':
                print sfile[0][len(listgroup[0]):]
            elif OUTPUT == 'basename':
                print os.path.basename(sfile[0])
