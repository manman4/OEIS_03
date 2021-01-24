def A(n)
  len = 1
  a = [[n]]
  # sigma(n) = 1
  while len < n - 1
    len += 1
    b = []
    a.each{|c|
      # sigma(n) = 1
      ([(n + len - 1) / len, 2].max..n).each{|num|
        if !c.include?(num)
          i = c.clone
          j = i + [num]
          b << j
        end
      }
    }
    a = b
  end
  a
end

p (1..15).map{|i| p A(i).size}