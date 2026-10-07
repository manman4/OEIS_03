"""Inject working-table/b-file I/O failures into a copy of the audited source."""
from pathlib import Path
import json, os, subprocess, sys, tempfile
source=Path(sys.argv[1]).read_text()
normal=Path(sys.argv[2]).resolve()
wrappers=r'''
static bool chosen_fd(int fd, const char* variable) {
    const char* target=std::getenv(variable);
    struct stat actual{}, wanted{};
    return target && !fstat(fd,&actual) && !stat(target,&wanted)
        && actual.st_dev==wanted.st_dev && actual.st_ino==wanted.st_ino;
}
static std::string fault_mode() {
    const char* mode=std::getenv("ULAM_AUDIT_FAULT"); return mode ? mode : "";
}
static ssize_t audit_pwrite(int fd,const void* data,size_t count,off_t offset) {
    static unsigned calls=0;
    auto mode=fault_mode();
    if(mode=="pwrite_enospc") {
        if(++calls==1) return ::pwrite(fd,data,std::min(count,size_t{3}),offset);
        errno=ENOSPC;return -1;
    }
    if(mode=="short_eintr") {
        if(++calls==1){errno=EINTR;return -1;}
        if(calls==2) count=std::min(count,size_t{17});
    }
    return ::pwrite(fd,data,count,offset);
}
static ssize_t audit_pread(int fd,void* data,size_t count,off_t offset) {
    static unsigned calls=0;
    auto mode=fault_mode();
    if(mode=="pread_eio"){errno=EIO;return -1;}
    if(mode=="short_eintr") {
        if(++calls==1){errno=EINTR;return -1;}
        if(calls==2) count=std::min(count,size_t{19});
    }
    return ::pread(fd,data,count,offset);
}
static int audit_ftruncate(int fd,off_t length) {
    if(fault_mode()=="truncate_eio"){errno=EIO;return -1;}
    return ::ftruncate(fd,length);
}
static ssize_t audit_write(int fd,const void* data,size_t count) {
    static unsigned calls=0;
    if(chosen_fd(fd,"ULAM_AUDIT_BFILE")) {
        auto mode=fault_mode();
        if(mode=="bfile_write_enospc") {
            if(++calls==1)return ::write(fd,data,std::min(count,size_t{2}));
            errno=ENOSPC;return -1;
        }
        if(mode=="short_eintr") {
            if(++calls==1){errno=EINTR;return -1;}
            if(calls==2)count=std::min(count,size_t{2});
        }
    }
    return ::write(fd,data,count);
}
static int audit_fsync(int fd) {
    auto mode=fault_mode();
    if((mode=="disk_fsync_eio" && chosen_fd(fd,"ULAM_AUDIT_DISK")) ||
       (mode=="bfile_fsync_eio" && chosen_fd(fd,"ULAM_AUDIT_BFILE"))) {
        errno=EIO;return -1;
    }
    return ::fsync(fd);
}
#define pwrite audit_pwrite
#define pread audit_pread
#define ftruncate audit_ftruncate
#define write audit_write
#define fsync audit_fsync
'''
split=source.index('using U =')
Path('io_injected.cpp').write_text(source[:split]+wrappers+source[split:])
compiler=['/usr/bin/xcrun','--sdk','macosx','clang++'] if sys.platform=='darwin' else ['c++']
subprocess.run([*compiler,'-std=c++17','-O1','-g','-fsanitize=undefined,bounds',
                '-fno-sanitize-recover=all','-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE',
                'io_injected.cpp','-o','io_injected'],check=True)
injected=Path('io_injected').resolve()
base=subprocess.run([str(normal),'--ram','--no-bfile','--no-summary','--terms','10000',
                     '--report','0'],capture_output=True,text=True,check=True,timeout=30)
base_stats=json.loads(base.stderr.splitlines()[-1])
records=[]
for mode in ['short_eintr','pwrite_enospc','pread_eio','truncate_eio',
             'bfile_write_enospc','disk_fsync_eio','bfile_fsync_eio']:
    with tempfile.TemporaryDirectory(prefix='ulam02-io-'+mode+'-') as tmp:
        d=Path(tmp)
        env=dict(os.environ,ULAM_AUDIT_FAULT=mode,ULAM_AUDIT_DISK=str(d/'cache.bin'),
                 ULAM_AUDIT_BFILE=str(d/'b.txt'))
        result=subprocess.run([str(injected),'--disk','cache.bin','--cache-mib','1',
                               '--bfile','b.txt','--summary','summary.json','--terms','10000',
                               '--dictionary-limit','1','--report','0'],cwd=d,env=env,
                              capture_output=True,text=True,timeout=30)
        assert 'runtime error:' not in result.stderr and 'UndefinedBehaviorSanitizer' not in result.stderr
        stats=json.loads((d/'summary.json').read_text())
        assert (d/'summary.json').read_text()==result.stderr.splitlines(keepends=True)[-1]
        if mode=='short_eintr':
            assert result.returncode==0 and stats['status']=='complete'
            assert result.stdout==base.stdout and (d/'b.txt').read_text()==base.stdout
            assert stats['digest64']==base_stats['digest64'] and stats['escaped_blocks']>0
        else:
            assert result.returncode==1 and stats['status']=='incomplete'
            assert '"status":"complete"' not in result.stderr
        records.append({'mode':mode,'exit_code':result.returncode,'summary':stats,
                        'bfile_contents':(d/'b.txt').read_text(),'stdout':result.stdout})
Path('io_results.json').write_text(json.dumps(records,indent=2)+'\n')
print('PASS short read/write and EINTR: exact normal output and digest, with raw escapes.')
print('PASS pwrite ENOSPC, pread/ftruncate EIO, b-file partial-write ENOSPC and both fsync failures: incomplete, saved JSON.')
