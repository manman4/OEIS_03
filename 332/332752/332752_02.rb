ary = [1,2,3] * 4
b = []
ary.permutation{|i|
  k11 = (2 * i.index(1) +     i.rindex(1)) / 3
  k12 = (    i.index(1) + 2 * i.rindex(1)) / 3
  m1 = -i.index(1) + i.rindex(1)
  if (m1 % 3 == 0) && (i[k11] == 1) && (i[k12] == 1)
    k21 = (2 * i.index(2) +     i.rindex(2)) / 3
    k22 = (    i.index(2) + 2 * i.rindex(2)) / 3
    m2 = -i.index(2) + i.rindex(2)
    if (m2 % 3 == 0) && (i[k21] == 2) && (i[k22] == 2)
      k31 = (2 * i.index(3) +     i.rindex(3)) / 3
      k32 = (    i.index(3) + 2 * i.rindex(3)) / 3
      m3 = -i.index(3) + i.rindex(3)
      if (m3 % 3 == 0) && (i[k31] == 3) && (i[k32] == 3)
        #k4 = i.index(4) + i.rindex(4)
        #m4 = -i.index(4) + i.rindex(4)
        #if (k4 % 2 == 0) && (i[k4 / 2] == 4)
          if m1 >= m2 && m2 >= m3 #&& m3 >= m4
            b << i
          end
        #end
      end
    end
  end
}
p b.uniq
p b.uniq!.size