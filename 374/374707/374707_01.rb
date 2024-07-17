def A(n, k)
  ary = []
  (0..n).each{|i|
    (i..n).each{|j|
      ary << i ** k + j ** k
    }
  }
  ary.uniq.size
end

p ary = (0..100).map{|i| A(i, 3)}
