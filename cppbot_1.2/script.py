def generate():
  count = 0;
  output = "";
  for i in range(250):
    output += "set<Card> set" + count + ";\n"
    count += 1;
  print output;

def add(cards, equity):
  output = ""
  for i in range(3):
    output += "c" + str(i) + ".rank = \'" + cards[i][0] + "\';\n"
    output += "c" + str(i) + ".suit = \'" + cards[i][1] + "\';\n"		
    output += "myset.insert(c" + str(i) + ");\n"
  output += "equity250[myset] = " + str(equity) + ";\n"
  output += "myset.clear();\n"
  print output

def run():
  output = "set<Card> myset;\n Card c0, c1, c2;\n"
  print output

  add(['Ac', 'Ad', 'Td'], 75.02)
  add(['Ac', 'Ad', '5d'], 74.94)
  add(['Ac', 'Ad', 'Jd'], 74.90)
  add(['Ac', 'Ad', '8d'], 74.81)
  add(['Ac', 'Ad', '9d'], 74.74)
  add(['Ac', 'Ad', '4d'], 74.63)
  add(['Ac', 'Ad', '7d'], 74.62)
  add(['Ac', 'Ad', '6d'], 74.60)
  add(['Ac', 'Ad', 'Kd'], 74.37)
  add(['Ac', 'Ad', 'Qd'], 75.33)
  add(['Ac', 'Ad', 'Th'], 74.15)
  add(['Ac', 'Ad', '2d'], 74.06)
  add(['Ac', 'Ad', '3d'], 73.90)
  add(['Ac', 'Ad', '8h'], 73.83)
  add(['Ac', 'Ad', '5h'], 73.77)
  add(['Kc', 'Kd', 'Ad'], 73.72)
  add(['Ac', 'Ad', 'Jh'], 73.65)
  add(['Ac', 'Ad', 'Kh'], 73.54)
  add(['Ac', 'Ad', '7h'], 73.53)
  add(['Ac', 'Ad', '4h'], 73.45)
  add(['Ac', 'Ad', '9h'], 73.42)
  add(['Ac', 'Ad', 'Qh'], 73.37)
  add(['Ac', 'Ad', '6h'], 73.31)
  add(['Ac', 'Ad', '3h'], 72.83)
  add(['Kc', 'Kd', 'Ah'], 72.76)
  add(['Ac', 'Ad', '2h'], 72.39)
  add(['Kc', 'Kd', 'Td'], 71.95)
  add(['Kc', 'Kd', 'Jd'], 71.70)
  add(['Kc', 'Kd', 'Qd'], 71.57)
  add(['Qc', 'Qd', 'Ad'], 71.56)
  add(['Kc', 'Kd', '6d'], 71.51)
  add(['Kc', 'Kd', '5d'], 71.40)
  add(['Kc', 'Kd', '7d'], 71.38)
  add(['Kc', 'Kd', '9d'], 71.25)
  add(['Kc', 'Kd', '8d'], 71.22)
  add(['Kc', 'Kd', '4d'], 71.17)
  add(['Qc', 'Qd', 'Kd'], 70.94)
  add(['Kc', 'Kd', '3d'], 70.79)
  add(['Kc', 'Kd', 'Th'], 70.72)
  add(['Kc', 'Kd', 'Jh'], 70.63)
  add(['Kc', 'Kd', '2d'], 70.57)
  add(['Kc', 'Kd', '9h'], 70.48)
  add(['Kc', 'Kd', 'Qh'], 70.38)
  add(['Qc', 'Qd', 'Ah'], 70.33)
  add(['Ac', 'Ad', 'Ah'], 70.17)
  add(['Kc', 'Kd', '7h'], 70.00)
  add(['Kc', 'Kd', '8h'], 70.00)
  add(['Kc', 'Kd', '6h'], 69.97)
  add(['Kc', 'Kd', '5h'], 69.89)
  add(['Kc', 'Kd', '4h'], 69.80)


if __name__ == "__main__":
  run()






































