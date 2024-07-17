def A(n)
  ary = []
  (0..n).each{|i|
    (i..n).each{|j|
      (j..n).each{|k|
        (k..n).each{|l|
          ary << i ** 3 + j ** 3 + k ** 3 + l ** 3
        }
      }
    }
  }
  ary.uniq.size
end

n = 50
b=[]
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
  b << j
}
p b
