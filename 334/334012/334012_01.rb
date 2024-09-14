def A(n)
  m = Math.sqrt(n).to_i
  ary = Array.new(n + 1, 0)
  (1..m).each{|i|
   
    sum = i * (3 * i - 2)
    if sum <= n
      ary[sum] += 1
      i += 1
      
      sum += i * (3 * i - 2)
      while sum <= n
        j = ary[sum]
        ary[sum] += 1
        i += 1
        
        sum += i * (3 * i - 2)
      end
    end
  }
  ary
end

a = A(2 * 10 ** 9)
p (1..10).map{|i| a.index(i)}
p (1..2 * 10 ** 5).select{|i| a[i] == 2}
p (1..2 * 10 ** 5).select{|i| a[i] == 3}

# [1, 1045, 5985, nil, nil, nil, nil, nil, nil, nil]
# [1045, 2205, 5461, 5915, 13335, 13736, 24030, 33129, 37030, 49665, 50154, 60030, 60564, 64710, 77940, 84336, 85085, 86130, 86360, 87899, 102069, 106785, 110635, 120414, 122009, 125460, 126896, 133695, 140140, 150705, 157815, 177155, 196929, 197885]
# [5985]


