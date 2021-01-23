\\ \r
\\ Nは使わない 
{a(n) = my(t='t); polcoef(n!*exp(sum(j=1, n, t*sigma(j)*(x^j+x*O(x^n))/j!)), n)};
M=100;
cnt = 1;
for(n=1, M, v=a(n); for (k=1, n, write("C:\\Users\\xxx\\Desktop\\b338871.txt", cnt, " ", polcoef(v, k)); cnt++))