isok(k) = my(sk=sigma(k)); fordiv(k, d, if (d*sigma(d) == sk, return(1)));
cnt = 1;
for(k=1, 89516065, if(isok(k), write("C:\\Users\\xxx\\Desktop\\b339472.txt", cnt, " ", k); cnt++))