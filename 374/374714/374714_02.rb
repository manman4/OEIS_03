def A(n)
  h = {}
  (1..n).each{|i|
    (i..n).each{|j|
      (j..n).each{|k|
        (k..n).each{|l|
          x = i * i * i + j * j * j + k * k * k + l * l * l
          if h.has_key?(x)
            h[x] += 1
          else
            h[x] = 1
          end
        }
      }
    }
  }
  h.size
end

n = 47
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