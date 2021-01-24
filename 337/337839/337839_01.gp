default(parisize, 12000000000)
nn = 30 * 10^5;
lista(nn) = {my(vd = vector(nn, k, digits(k)), vs = vector(nn, k, vecsum(vd[k])), vp = vector(nn, k, vecprod(vd[k])), vsp = Set(vector(nn, k, k+vp[k])), vss = Set(vector(nn, k, k+vs[k])), vps = Set(vector(nn, k, k*vs[k])), vpp = Set(vector(nn, k, k*vp[k])), vk = vector(nn, k, k));
v=setintersect(vk, setintersect(vsp, setintersect(vss, setintersect(vps, vpp)))); }
v=lista(nn);
for(n=1, 1000, write("C:\\Users\\xxx\\Desktop\\b337839.txt", n, " ", v[n]))
