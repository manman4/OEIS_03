ary = [1,2,3,4] * 3
b = []
ary.permutation{|i|
  k1 = i.index(1) + i.rindex(1)
  m1 = -i.index(1) + i.rindex(1)
  if (k1 % 2 == 0) && (i[k1 / 2] == 1)
    k2 = i.index(2) + i.rindex(2)
    m2 = -i.index(2) + i.rindex(2)
    if (k2 % 2 == 0) && (i[k2 / 2] == 2)
      k3 = i.index(3) + i.rindex(3)
      m3 = -i.index(3) + i.rindex(3)
      if (k3 % 2 == 0) && (i[k3 / 2] == 3)
        k4 = i.index(4) + i.rindex(4)
        m4 = -i.index(4) + i.rindex(4)
        if (k4 % 2 == 0) && (i[k4 / 2] == 4)
          if m1 >= m2 && m2 >= m3 && m3 >= m4
            b << i
          end
        end
      end
    end
  end
}
p b.uniq
p b.uniq!.size