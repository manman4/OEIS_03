M=60;

tabf(nn) = if(nn==1, print1(1, ", "), my(f=factor(nn)); for(i=1, #f~, for(j=1, 2, if((k=f[i, j])>j-1, print1(k, ", ")))));
for(nn=1, M, tabf(nn));