#!/usr/bin/env python
# High level interface tests

from __future__ import division,unicode_literals,print_function,absolute_import
from pentago import *
from geode import *
import sys

def board_test(slice,boards,wins):
  boards,wins = map(ascontiguousarray,(boards,wins))
  assert len(boards)==len(wins)
  init_supertable(15)
  print('slice %d, samples %d'%(slice,len(boards)))
  counts = high_board_t.sample_check(reader_block_cache((),1),boards,wins)
  print('counts: loss %d, tie %d, win %d'%tuple(counts))

def test_board_33():
  boards = asarray([2770068518349711244,2770030018707007453,2792799663139343586,2770023185559136596,2770031367330090210,946362534908277485,2770036521735166202,2156986100076914650,2362474322681010399,2770004605403015769,2770864247678506424,923533499761498528,945229144863549861,2226209450043060318,2846036856505043064,2154465015406988820,930880028643439236,925532119476742730,2747219245982165682,2109134049064989235,2770867679080943565,675020434118423253,2770050427979306820,2154455037858027882,2770885576634731260,239309724833949408,2770042203817641597,2154462309715611082,307694894062512714,741447876250707579,2770866747025267019,810412672450373585],dtype=uint64)
  wins = asarray([2536128238408835890,2536128238408835890,2536128238408835890,2536128238408835890,14757395258967641292,14757395258967641292,14757395258967641292,14757395258967641292,562958543486976,562958543486976,562958543486976,562958543486976,17289582787572068336,17289582787572068336,17289582787572068336,17289582787572068336,280379743272960,280379743272960,280379743272960,280379743272960,1095233372415,1095233372415,1095233372415,1095233372415,18446462599019167744,18446462599019167744,18446462599019167744,18446462599019167744,262709978263278,262709978263278,262709978263278,262709978263278,18374686483949879040,18374686483949879040,18374686483949879040,18374686483949879040,0,0,0,0,16842623,280920942706688,16842623,280920942706688,18446744073692708864,18446462603011031039,18446744073692708864,18446462603011031039,1229782938247303441,1229782938247303441,1229782938247303441,1229782938247303441,0,0,0,0,8224261361801232384,8224173400871039522,8224261361801232384,125490927137314,65535,243941257510912,65535,8608349216464240640,13767433141282127743,13776440340538965887,12836032443129708415,13767379883687644943,67553994426286080,58546795169447936,998954692578705408,67678170520776944,1808494317973082112,1808494317973082112,1808494317973082112,1808494317973082112,7378697629483820646,7378697629483820646,7378697629483820646,7378697629483820646,4611949243809530112,17288831082386817280,17289019241961357568,17288831082386817280,13834776580305514222,206411833339630,3221221102,206411833339630,1099528405248,72058693566333184,1099528405248,72058693566333184,18446462598732840960,0,18446462598732840960,0,18446481367739858944,17294086455937798144,17294086455919906816,18446462603027742720,65535,1152657617771696127,1152657617789587455,17587891142655,17293822573129297920,4293918720,983040,1085086035472224015,1152657617789587455,18446744069414649855,18442521884633399280,17361641481138401520,278931492878763,13334030053253363979,278931492878763,13334030053253363979,18446462598732840960,67553994410557680,18446462598732840960,67553994410557680,0,842137600,842137600,0,18446744073709551615,18446744072850571263,18446744072850571263,18446744073709551615,2533876404234887978,2533876404234887978,2533876404234887978,38663885562666,0,0,0,18446462598732840960,16369037670802391296,9297681434517569792,9297823305877291264,9297931204045701376,112588272697344,7378697627765833728,7378697627765833728,7378585039493136384,0,0,0,0,281470681808895,281470681808895,281470681808895,281470681808895,286265616,286265616,6582573805641685850,100443637813520,18446744073423220462,18446744073423220462,281474690318336,18446462598732902126,4629771061704314060,8088553168516218847,8088553168516218847,4629771061704327135,13527612320720302899,10068795029536309248,10068795029536309248,13527612320720289792,17294090759477194752,62218,0,63051356855861248,1152640029898510335,18446744073709486080,18446744073709551615,18379189048491114255,9277555973092507648,9277555973092507648,9277555973092507648,9277555973092507648,3689348813882929971,3689348813882929971,3689348813882929971,3689348813882929971,17298044697470374319,18378916304620687791,17298044697470374319,18378916304620687791,1148699375661219840,67818972417884160,1148699375661219840,67818972417884160,4919131752989261823,4919131756138856447,4919131756138808388,4919131756138856447,13527612320720289792,13527612317570695168,13527612317570743227,13527612317570695168,17942200792136581263,17942200794426966159,71776183485726720,17365598752588955663,33554432,2201021906688,18374966859431673855,4278255360,2459528347070234624,2459528347070234624,2459528347070234624,2459528347070234624,15987215726639120384,15987215726639120384,15987215726639120384,15987215726639120384,2459565876494606882,2459584641493054259,2459584641493054259,2459565876494606882,15987178197214944733,15987159432216497356,15987159432216497356,15987178197214944733,2290480879370240,2535002321321861888,2534964774144320256,74348074917298432,8608349212742057983,0,112592281272320,7378585039493197550,281474976645120,281471540723712,153417090793472,281474976645120,7378585039493201919,7378585042928336880,7378695432178565119,7378585039493201919,6196953084684813380,6196953084684861439,4919131753561915391,6196953084684861439,2308549563,2308505600,13527612320147636224,2308505600,18694913266944,1513228169709753600,1513228169709753600,1513228169709753600,16910711684942064302,16910711684942064302,16910711684942064302,16910711684942064302],dtype=uint64).reshape(-1,2,4)
  board_test(33,boards,wins)

def test_board_34():
  boards = asarray([2770041146411456919,2770038673513783845,2770050553393319851,2770013546935307490,923292882872380976,900481122287364025,946112525136438820,2770041731530493931,2802982536206887515,1128478618662948066,2748082826466439593,2770038711881834808,2770041700430007522,2792847903933075378,948614450817011908,2771194894743119477,2770321114698889945,2978042358059443945,2093653277386296546,2770320139751928506,2155296512342246484,2770309947511413149,2838721586901232013,925826497389862464,2230435388789368470,2792848973808150789,2770884996525730486,315294593736126568,2086053668358079332,2770008792978574562,3000569749847548417,2770853913281178267],dtype=uint64)
  wins = asarray([52777364500480,52777364500480,52777364500480,52777364500480,18446462598732840960,18446462598732840960,18446462598732840960,18446462598732840960,0,0,0,0,0,0,0,0,15986934256530030592,15986934256530030592,15986934256530030592,15986934256530030592,2459659699482556279,2459659699482556279,2459659699482556279,2459659699482556279,1080880403494993920,1080880403494993920,1080880403494993920,1080880403494993920,61695,61695,61695,61695,7455410111385593718,7455410111385593718,7455410111385593718,7455410111385593718,9838113385990848512,9838113385990848512,10991333962323918848,9838113385990848512,37529424232448,37529424232448,37529424232448,37529424232448,7378585039493136384,7378585039493136384,7378660098341601280,7378585039493201919,17294051271547867136,17294051271547867136,17294051271547867136,17294051271547867136,1152657617789587455,1152657617789587455,1148435428713435120,1148435428713435120,7378697629483820646,7378697629483820646,7378697629483820646,7378697629483820646,0,0,0,0,13401324889025745403,18302626686576885247,18302626686576885247,651615718060523787,144117387132666368,144117387132666368,144117387099111936,17505758868271067888,17294086455919964160,17294086455919964160,17294086455919964160,17294086455919964160,0,0,0,0,1229782938247303441,1229782938247303441,1229782938247303441,1229782938247303441,187647121162240,0,61166,0,72621652109820162,72621652109820162,72621652109820162,72621652109820162,0,0,0,0,1080880403494997760,1080880403494997760,1080880403494997760,1080880403494997760,0,0,0,0,5787219346163324245,5787219346163324245,5787219346163324245,5787219346163324245,12659337077561753600,12659337077561753600,12659337077561753600,12659337077561753600,6922154808067443724,6917651208389741580,6922154808067443724,6922154808067443724,1148136430017445888,11524589265642061824,11520367140991448048,11524429973894987776,823989698560,18140494625479973273,18140494625479973273,53762540900778137,18446556332099108864,306249447656916036,306249447585611776,4918005835902239812,7608384715226507670,7608384715226507670,7608384715226507670,7608384715226507670,0,38505,38505,0,32637851408790515,32637851408790515,8319261165770503155,32637851408790387,18269977788402633740,18269977788402633740,10127469662116776972,18269977788402633740,1095233372160,1095233372160,1095233372160,1095233372160,18446462598732844800,1152656522556215040,16492926074880,18442256967008259840,2533876404234879240,2533876404234879240,2533876404234879240,2533876404234879240,15911460269411270656,14753792379265806048,15911217483499962368,14753792379265744896,5764695485306654720,5764695485306654720,5764695485306654720,5764695485306654720,12682048588402896895,0,12682048588402896895,0,9223512780785680385,9223512780785680385,9223512780785680385,9223512780785680385,140728898453502,9223231290776485888,140728898420736,140728898453502,12371013993126341550,12371013993126341550,12371013993126341550,12371013993126341550,92706869084160,21585,6075730080583188480,21585,1230045648225566719,1230045643930599424,1230045648225566719,1230045648225566719,9838113385990848512,9838113388567789568,9838113385990848512,9838113385990848512,18446518893728235519,18446518893728235519,18446518893728235519,18446518893728235519,225179981316096,225179981316096,225179981316096,225179981316096,17293945718699913216,17293945718699913216,17293945718699913216,17293945718699913216,1152798355009572863,1152798355009572863,1152798355009572863,1152798355009572863,1253708676345500006,1253708676345500006,1253708676345500006,1253708676345500006,262340897406976,17152502378239361024,262340897406976,261722422116352,9007336695791648,9007336695791648,9007336695791648,9007336695791648,18437455399478099968,18437455399478165471,281333242789888,18437736737013759967,0,0,0,0,0,0,0,0,0,0,0,0,17361376567555584240,17361376567555584240,17361376567555584240,17361376567555584240,18302628884023083007,10955456453541468569,18302628884023083007,10955456453541468569,0,7378697629483820646,0,7378697629483820646,7071322264739930658,7071322264739930658,7071322264739930658,7071322264739930658,0,11375248236456287709,0,40413],dtype=uint64).reshape(-1,2,4)
  board_test(34,boards,wins)

def test_board_35():
  boards = asarray([2770019925981538179,2770042195386967854,2770323566422730384,2778483158587085068,2793685590919486989,924129152089597800,2747238401536305845,2770320551739463984,2770013861057737266,925814677501848135,2770037990613979305,2770037169701932258,2770031255531375842,1538878880271510170,2770039652617695822,2770036856742619821,2770042203971587397,2178099867909239292,2773419516290410208,923280551968197858,2793121416327079808,2770031655089550115,2770020119255069154,1561665490393376651,2792841560536915794,2770014011381589737,2792811942883242736,2245649648803466466,2770037642004810978,2711772326407514067,2223695799075023191,2792841642444072643],dtype=uint64)
  wins = asarray([3689348814741910323,3689348814741910323,3689348814741910323,3689348814741910323,1145324612,1145324612,1145324612,1145324612,2863267840,2863267840,2863267840,2863267840,18446744070846218240,18446744070846218240,18446744070846218240,18446744070846218240,2576478208,12731869663199342592,11574426966779076608,12731869663199358976,18446744070559891456,4919131752989213764,6148914690950190421,4919131752989196288,1407397718751637,415155834157461,133680857119125,1342468100,18442521880339415040,18442240478376099840,18446462603027742720,18446744069414584320,0,150117696990958,150117696990958,150117696990958,18446593956012556288,18446593956012556288,13527462203023360000,18446593956012556288,13527405908887059387,13527405908887059387,572688520,13527405908887059387,4919150517701329988,4919131752989196288,18446744073136863095,75059993789508,0,0,0,0,0,0,0,0,9223550157741250047,14144962000348398668,9225905307352978943,14144962000348403199,6761413585405018112,1842253723000191923,2149624397568212992,1842253723000176640,1360,1360,1360,1360,18446744073709486080,18446744073709486080,18446744073709486080,18446744073709486080,3691675708902438673,16607536724670225,3691675674542669824,3691675708902408192,4899916394579099648,4919206810692354048,4919206810692419583,4919056692995489791,2459565876494598144,2459565876494598144,2459565876494598144,2459565876494598144,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4629771064853872704,286326784,286326784,4629771064853872704,0,0,0,0,649091200623577346,649091200623577346,649091200623577346,649091200623577346,0,0,0,0,0,0,0,0,0,0,0,0,14757170080131610487,14757170080131610487,14757170080131610487,14757170080131610487,3689348814741897216,3689348814741897216,3689348814741897216,3689348814741897216,67371008,67436543,67371008,67436543,18446667910801832634,18446667910801784832,18446667910801832634,18446667910801784832,858993459,855651072,858980352,858980352,8608630683423735808,18446744072850505728,18446744069414649855,7378866511320213094,17294086455919902720,17294086455919902720,17293822573129236480,17294086455919902720,0,0,281470681743360,65535,550395609011,9347995730944,3458826638374433792,3458826638209482675,18158359181174571008,18158359177519496191,864413463114419199,864413463145676800,9223512776550088704,9223512776550088704,9223512776550088704,9223512776550088704,9219009105996316656,9219009105996316656,9219009105996316656,9219009105996316656,12297641735351872170,12297641735351872170,12297641735351872170,12297641735351872170,0,0,0,0,7378697629483794432,7378697629483820646,0,7378697629483794432,13107,0,7378678863053717504,13107,8014824041049243648,8014824041049243648,8014824041049243648,8014824041049243648,17523733954560,17523733954560,17523733954560,17523733954560,35184372097024,35184372097024,35184372097024,35184372097024,6148820870002368511,6148820870002368511,6148820870002368511,6148820870002368511,72620543991349506,72620543991349506,72620543991349506,72620543991349506,281474976645120,281474976645120,281474976645120,281474976645120,263886817259520,263886817259520,4026593280,263886817198080,1152640029630140400,17587891077120,280375465082880,18374686479671688960,0,0,0,0,0,0,0,0,2459565876494606882,572662306,37529996894754,572662306,0,8608480565726806016,8608349212742049245,8608480565726806016,1225466917020569857,1225466917020569861,1225466917020569857,1225541700991127809,7378660102350237422,316452936679817216,7378660102350237422,7378585043501772526,4290510779,4290510779,4290510779,4290510779,18446744069414584320,18446744069414584320,18446744069414584320,18446744069414584320],dtype=uint64).reshape(-1,2,4)
  board_test(35,boards,wins)

def bootstrap(slice,n,verbose=1):
  samples = load('../../data/edison/project/all/sparse-%d.npy'%slice)
  random.seed(81311)
  random.shuffle(samples)
  samples = samples[:n]
  if verbose:
    print('boards[%d] = asarray(%s,dtype=uint64)'%(slice,compact_str(samples[:,0])))
    print('wins[%d] = asarray(%s,dtype=uint64).reshape(-1,2,4)'%(slice,compact_str(samples[:,1:].ravel())))
  return samples[:,0].copy(),samples[:,1:].reshape(-1,2,4).copy()

if __name__=='__main__':
  data = None
  if '--bootstrap' in sys.argv:
    for slice in 33,34,35:
      bootstrap(slice,n=32)
  elif len(sys.argv)==2:
    for slice in 33,34,35:
      board_test(slice,*bootstrap(slice,n=int(sys.argv[1]),verbose=0))
  else:
    test_board_33()
    test_board_34()
    test_board_35()