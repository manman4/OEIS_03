isok(n) = numdiv(n)>numdiv(n+1) && numdiv(n+1)>numdiv(n+2) && numdiv(n+2)>numdiv(n+3);
for(n=1, 4250, if(isok(n), print1(n,", ")))   
