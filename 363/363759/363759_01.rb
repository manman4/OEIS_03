def A(n)
  a = []
  (1..2 ** (n + 1)).each{|i|
    (1..2 ** n).each{|j|
      a << i ** n + j ** (n + 1)
    }
  }
  a.group_by{|i| i}.select{|k, v| v.size == 2}.map{|k, v| v}
end

(1..11).each{|i|
  j = A(i).sort
  p [j.size, j]
}

(1..11).each{|i|
  p A(i).sort[0][0]
}
