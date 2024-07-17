def A(n)
  ary = []
  (1..n).each{|i|
    (i..n).each{|j|
      (j..n).each{|k|
        ary << i ** 2 + j ** 2 + k ** 2 
      }
    }
  }
  ary.uniq.size
end

n = 54
b=[]
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
  b << j
}
p b
