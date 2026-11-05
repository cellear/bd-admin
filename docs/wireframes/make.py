# Generates SVG wireframes of the planned Drupal wp-admin pages.
from html import escape as e

W, H = 1280, 820
SB = 230   # sidebar width
FONT = "font-family='Inter, Helvetica, Arial, sans-serif'"

def svg_open(title):
    return [f"<svg xmlns='http://www.w3.org/2000/svg' width='{W}' height='{H}' viewBox='0 0 {W} {H}' {FONT} font-size='13'>",
            f"<title>{e(title)}</title>",
            f"<rect width='{W}' height='{H}' fill='#f3f4f9'/>"]

def rect(x,y,w,h,fill='#fff',stroke='#c9ccd6',rx=6,sw=1,dash=None):
    d = f" stroke-dasharray='{dash}'" if dash else ''
    return f"<rect x='{x}' y='{y}' width='{w}' height='{h}' rx='{rx}' fill='{fill}' stroke='{stroke}' stroke-width='{sw}'{d}/>"

def text(x,y,s,size=13,fill='#222330',weight='normal',anchor='start'):
    return f"<text x='{x}' y='{y}' font-size='{size}' fill='{fill}' font-weight='{weight}' text-anchor='{anchor}'>{e(s)}</text>"

def link(x,y,s,size=13): return text(x,y,s,size,'#003ecc')
def muted(x,y,s,size=12): return text(x,y,s,size,'#6b6f82')

def button(x,y,label,primary=True,w=None):
    w = w or (len(label)*7+28)
    fill, fg, st = ('#003ecc','#fff','#003ecc') if primary else ('#fff','#003ecc','#003ecc')
    return rect(x,y,w,32,fill,st,4) + text(x+w/2,y+21,label,13,fg,'600','middle')

def input_(x,y,w,h=34,placeholder=''):
    return rect(x,y,w,h,'#fff','#8e929c',4) + (muted(x+10,y+h/2+4,placeholder) if placeholder else '')

def select(x,y,w,val):
    return rect(x,y,w,34,'#fff','#8e929c',4) + text(x+10,y+22,val) + text(x+w-18,y+22,'▾',13,'#555')

def sidebar(active):
    items = [('Dashboard','▣'),('Posts','✎'),('Media','▦'),('Pages','▤'),('Recipes','♨'),('Comments','☰'),
             ('Appearance','◐'),('Plugins','⚙'),('Users','☺'),('Tools','✇'),('Settings','⚒')]
    out = [rect(0,0,SB,H,'#fff','#d4d6de',0)]
    out.append(rect(16,16,120,34,'#e8ebf5','#e8ebf5',6)); out.append(text(28,38,'Umami','14','#222330','700'))
    y = 76
    for label,icon in items:
        if label==active: out.append(rect(10,y-20,SB-20,32,'#e6ebf7','#e6ebf7',6))
        out.append(text(24,y,icon,14,'#4a4f66')); out.append(text(52,y,label,14,'#222330','600' if label==active else '500'))
        if label!='Comments': out.append(text(SB-26,y,'›',14,'#8e929c'))
        y += 38
    out.append(text(24,H-56,'◯',14,'#4a4f66')); out.append(text(52,H-56,'admin',14))
    out.append(text(24,H-26,'?',14,'#4a4f66')); out.append(text(52,H-26,'Help',14))
    return out

def panel(x,y,w,h,title):
    return [rect(x,y,w,h), rect(x,y,w,36,'#f6f7fb','#c9ccd6',6), f"<rect x='{x}' y='{y+30}' width='{w}' height='6' fill='#f6f7fb'/>",
            text(x+14,y+23,title,14,'#222330','700')]

def h1(s): return text(SB+36,56,s,26,'#222330','700')

# ---------- 1. Dashboard ----------
s = svg_open('Dashboard (/wp-admin)') + sidebar('Dashboard')
s.append(h1('Dashboard'))
X=SB+36; CW=W-X-36
s += panel(X,84,CW,110,'Welcome to Drupal!')
s.append(link(CW+X-70,107,'Dismiss'))
for i,(t,l) in enumerate([('Author rich content','→ Add an article'),('Customize your site','→ Block layout'),('Switch the look & feel','→ Appearance')]):
    cx = X+20+i*(CW//3)
    s.append(text(cx,140,t,14,'#222330','600')); s.append(link(cx,166,l))
L = X; LW = CW//2-12; R = X+CW//2+12; RW = CW-CW//2-12
s += panel(L,212,LW,86,'Site Health Status')
s.append(text(L+16,262,'● 0 errors, 2 warnings',13,'#222330','600')); s.append(link(L+16,286,'Visit the Site Health screen →'))
s += panel(L,314,LW,176,'At a Glance')
for i,t in enumerate(['✎ 8 Published posts','♨ 10 Published recipes','▤ 1 Published page','☰ 0 Comments']):
    s.append(link(L+16,362+i*24,t))
s.append(muted(L+16,472,'Drupal 11.4.11 running the Umami theme.'))
s += panel(L,506,LW,290,'Activity')
s.append(text(L+16,556,'Recently Published',13,'#222330','600'))
for i,(d,t) in enumerate([('Nov 3','Give your oatmeal the ultimate makeover'),('Nov 2','Skip the spirits with delicious mocktails'),('Nov 1','Baking mishaps – our troubleshooting tips'),('Oct 31','Let’s hear it for carrots')]):
    s.append(muted(L+16,582+i*22,d)); s.append(link(L+76,582+i*22,t))
s.append(text(L+16,696,'Recent Comments',13,'#222330','600')); s.append(muted(L+16,720,'No comments yet.'))
s += panel(R,212,RW,278,'Quick Draft')
s.append(text(R+16,262,'Title')); s.append(input_(R+16,270,RW-32))
s.append(text(R+16,330,'Content')); s.append(input_(R+16,338,RW-32,80,'What’s on your mind?'))
s.append(button(R+16,432,'Save Draft')); s.append(muted(R+130,453,'Creates an Article in moderation state “Draft”'))
s += panel(R,506,RW,290,'Drupal Events and News')
for i,t in enumerate(['WordPress shows a live feed of meetups and news here.','Drupal’s nearest equivalent is the Announcements feed,','plus drupal.org news and DrupalCon events.']):
    s.append(text(R+16,556+i*20,t))
for i,t in enumerate(['→ Announcements (admin/announcements_feed)','→ drupal.org/news','→ events.drupal.org']):
    s.append(link(R+16,636+i*24,t))
s.append('</svg>'); open('1-dashboard.svg','w').write('\n'.join(s))

# ---------- 2. Posts list ----------
s = svg_open('Posts (/wp-admin/posts)') + sidebar('Posts')
s.append(h1('Posts')); s.append(button(X+90,32,'+ Add Post',False))
s.append(muted(X+210,52,'local action → /node/add/article'))
y=92
for i,t in enumerate(['All (9)','Published (8)','Drafts (1)','Archived (0)']):
    s.append(text(X+i*110,y,t,13,'#222330' if i==0 else '#003ecc','700' if i==0 else 'normal'))
s.append(muted(X+460,y,'← moderation_state exposed filter rendered as links'))
y=116
s.append(select(X,y,150,'Status: All')); s.append(select(X+162,y,170,'Category: All')); s.append(input_(X+344,y,220,34,'Search posts…')); s.append(button(X+576,y,'Filter',False))
y=170
s.append(select(X,y,160,'Bulk action')); s.append(button(X+172,y,'Apply',False))
# table
ty=222; cols=[('',30),('Title',360),('Author',130),('Categories',130),('Tags',130),('☰',40),('Status',90),('Date',100)]
s.append(rect(X,ty,CW,36,'#f6f7fb','#c9ccd6',0))
cx=X+10
for name,w in cols:
    s.append(text(cx,ty+23,name,13,'#222330','700')); cx+=w
rows=[('Give your oatmeal the ultimate makeover','Umami','Baking','oats, breakfast','0','Published','2026-11-03'),
      ('Skip the spirits with delicious mocktails','Holly Foat','—','drinks','0','Published','2026-11-02'),
      ('Baking mishaps – our troubleshooting tips','Margaret H.','Baking','baking, tips','0','Published','2026-11-01'),
      ('My new draft','admin','—','—','0','Draft','2026-11-05'),
      ('Let’s hear it for carrots','Gregorio S.','—','vegetables','0','Published','2026-10-31')]
ry=ty+36
for r in rows:
    s.append(rect(X,ry,CW,54,'#fff','#e2e4ea',0))
    s.append(rect(X+10,ry+18,14,14,'#fff','#8e929c',3))
    cx=X+10+30
    s.append(link(cx,ry+22,r[0],13)); s.append(muted(cx,ry+42,'Edit | Delete | View'))
    cx+=360
    for v,w in zip(r[1:],[130,130,130,40,90,100]):
        s.append(text(cx,ry+32,v)); cx+=w
    ry+=54
s.append(muted(X,ry+30,'9 items')); s.append(text(X+CW-60,ry+30,'‹ 1 ›'))
s.append(muted(X,ry+70,'Same View serves Pages (Basic page) and Recipes (Recipe; column “Recipe category”).'))
s.append('</svg>'); open('2-posts.svg','w').write('\n'.join(s))

# ---------- 3. Categories ----------
s = svg_open('Categories (/wp-admin/posts/categories)') + sidebar('Posts')
s.append(h1('Categories'))
L=X; LW=340; R=X+LW+24; RW=CW-LW-24
s += panel(L,84,LW,560,'Add New Category')
yy=130
s.append(text(L+16,yy,'Name')); s.append(input_(L+16,yy+8,LW-32)); s.append(muted(L+16,yy+60,'The name is how it appears on your site.'))
yy+=90; s.append(text(L+16,yy,'Slug')); s.append(input_(L+16,yy+8,LW-32)); s.append(muted(L+16,yy+60,'Becomes the term’s URL alias.'))
yy+=90; s.append(text(L+16,yy,'Parent Category')); s.append(select(L+16,yy+8,LW-32,'None'))
yy+=70; s.append(text(L+16,yy,'Description')); s.append(input_(L+16,yy+8,LW-32,90))
yy+=130; s.append(button(L+16,yy,'Add New Category'))
s.append(muted(L+16,yy+56,'Drupal’s own term add form, embedded.'))
s.append(input_(R+RW-260,84,180,34,'Search Categories')); s.append(button(R+RW-70,84,'Search',False,70))
ty=130; cols=[('',30),('Name',300),('Description',180),('Slug',130),('Count',60)]
s.append(rect(R,ty,RW,36,'#f6f7fb','#c9ccd6',0))
cx=R+10
for n,w in cols: s.append(text(cx,ty+23,n,13,'#222330','700')); cx+=w
rows=[('Uncategorized','—','uncategorized','0'),('Baking','Cakes, bread and pastry','baking','2'),('— Bread','','baking/bread','1'),('Seasonal','','seasonal','3'),('— Autumn','','seasonal/autumn','1')]
ry=ty+36
for r in rows:
    s.append(rect(R,ry,RW,54,'#fff','#e2e4ea',0)); s.append(rect(R+10,ry+18,14,14,'#fff','#8e929c',3))
    cx=R+40; s.append(link(cx,ry+22,r[0])); s.append(muted(cx,ry+42,'Edit | Delete | View'))
    cx+=300
    for v,w in zip(r[1:],[180,130,60]): s.append(text(cx,ry+32,v)); cx+=w
    ry+=54
s.append(link(R,ry+30,'Reorder categories with drag and drop (taxonomy overview) →'))
s.append(muted(R,ry+60,'Tags and Recipe Categories use this same page with another vocabulary.'))
s.append('</svg>'); open('3-categories.svg','w').write('\n'.join(s))

# ---------- 4. General settings ----------
s = svg_open('General Settings (/wp-admin/settings/general)') + sidebar('Settings')
s.append(h1('General Settings'))
rows=[('Site Title','Umami Food Magazine','system.site: name',True),
      ('Tagline','','system.site: slogan',True),
      ('WordPress Address (URL)','http://127.0.0.1:8081','read-only; set in settings.php / web server',False),
      ('Site Address (URL)','http://127.0.0.1:8081','read-only',False),
      ('Administration Email Address','admin@example.com','system.site: mail',True),
      ('Membership','☐ Anyone can register','user.settings: register',True),
      ('New User Default Role','Subscriber','disabled: Drupal assigns roles explicitly → Roles',False),
      ('Site Language','English','system.site: default_langcode',True),
      ('Timezone','UTC','system.date: timezone.default',True),
      ('Date Format / Time Format','→ Date and time formats','link to /admin/config/regional/date-time',True),
      ('Week Starts On','Monday','system.date: first_day',True)]
yy=100
for lab,val,note,ok in rows:
    s.append(text(X,yy+22,lab,13,'#222330','600'))
    fill = '#fff' if ok else '#eef0f5'
    if val.startswith('→'): s.append(link(X+260,yy+22,val))
    elif val.startswith('☐'): s.append(rect(X+260,yy+8,16,16,'#fff','#8e929c',3)); s.append(text(X+284,yy+22,val[2:]))
    else: s.append(rect(X+260,yy,360,34,fill,'#8e929c',4)); s.append(text(X+270,yy+22,val,13,'#222330' if ok else '#6b6f82'))
    s.append(muted(X+640,yy+22,note))
    yy+=52
s.append(button(X,yy+10,'Save Changes'))
s.append('</svg>'); open('4-settings-general.svg','w').write('\n'.join(s))
print('ok')
