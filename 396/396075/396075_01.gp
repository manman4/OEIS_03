q_binomial(n,r) = if(r < 0 || r > n, 0, prod(i=1, r, 1 - q^(n-i+1)) / prod(i=1, r, 1 - q^i));


my(N=70, q='q+O('q^N)); Vec(sum(j=1, 0, (-1)^(j-1) * q^(j*(j-1)/2+0) / prod(k=1, 0-j, (1-q^k))))

my(N=70, q='q+O('q^N)); Vec(sum(j=1, 0, q^(j*(j-1)+0) * q_binomial(-1, j-1) / prod(k=1, j-1, (1-q^k))))


my(N=70, q='q+O('q^N)); Vec(sum(j=1, 2, (-1)^(j-1) * q^(j*(j-1)/2+1) / prod(k=1, 2-j, (1-q^k))))

my(N=70, q='q+O('q^N)); Vec(sum(j=1, 2, q^(j*(j-1)+1) * q_binomial(1, j-1) / prod(k=1, j-1, (1-q^k))))


my(N=70, q='q+O('q^N));     sum(j=1, 4, (-1)^(j-1) * q^(j*(j-1)/2+2) / prod(k=1, 4-j, (1-q^k)))
my(N=70, q='q+O('q^N)); Vec(sum(j=1, 4, (-1)^(j-1) * q^(j*(j-1)/2+2) / prod(k=1, 4-j, (1-q^k))))

my(N=70, q='q+O('q^N)); Vec(sum(j=1, 4, q^(j*(j-1)+2) * q_binomial(3, j-1) / prod(k=1, j-1, (1-q^k))))


my(N=70, q='q+O('q^N)); Vec(sum(j=1, 6, (-1)^(j-1) * q^(j*(j-1)/2+3) / prod(k=1, 6-j, (1-q^k))))

my(N=70, q='q+O('q^N)); Vec(sum(j=1, 6, q^(j*(j-1)+3) * q_binomial(5, j-1) / prod(k=1, j-1, (1-q^k))))


my(N=70, q='q+O('q^N)); Vec(sum(j=1, 8, (-1)^(j-1) * q^(j*(j-1)/2+4) / prod(k=1, 8-j, (1-q^k))))

my(N=70, q='q+O('q^N)); Vec(sum(j=1, 8, q^(j*(j-1)+4) * q_binomial(7, j-1) / prod(k=1, j-1, (1-q^k))))

print("Table");

T(n,k) = my(q='q+O('q^(n+1))); polcoef(sum(i=1, 2*k, (-1)^(i-1) * q^(i*(i-1)/2+k) / prod(j=1, 2*k-i, (1-q^j))), n);
for(n=0, 20, for(k=0, n, print1(T(n,k),", "))); 

for(n=0, 50, print1(T(n,1),", ")); 
for(n=0, 50, print1(T(n,2),", ")); 
for(n=0, 50, print1(T(n,3),", ")); 
for(n=0, 50, print1(T(n,4),", ")); 

S(n,k) = my(q='q+O('q^(n+1))); polcoef(sum(i=1, 2*k, q^(i*(i-1)+k) * q_binomial(2*k-1, i-1) / prod(j=1, i-1, (1-q^j))), n);
for(n=0, 20, for(k=0, n, print1(S(n,k),", ")); print); 
for(n=0, 20, for(k=0, n, print1(T(n,k)-S(n,k),", ")));