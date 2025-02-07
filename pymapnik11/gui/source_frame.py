import wx, wx.stc
from .gui_frame import SourceFileBase
from . import scite_colors

CODEFOLDING_CIRCULAR=False
#BACK_COLOUR=wx.Colour(0x2a, 0x21, 0x1c)
#FORE_COLOUR=wx.Colour(0xb7, 0xb7, 0xb7)

FORE_COLOUR=wx.Colour(0x00, 0x00, 0x00)
BACK_COLOUR=wx.Colour(0xff, 0xff, 0xff)

def InitCodeFolding(TextArea):

    TextArea.SetMarginType(0, wx.stc.STC_MARGIN_NUMBER)
    TextArea.SetMarginMask(0, 0)
    TextArea.SetMarginSensitive(0, True)
    
    TextArea.SetMarginWidth(0, 30)


    TextArea.StyleSetForeground(wx.stc.STC_STYLE_LINENUMBER, FORE_COLOUR)
    TextArea.StyleSetBackground(wx.stc.STC_STYLE_LINENUMBER, BACK_COLOUR)
    TextArea.StyleSetForeground(wx.stc.STC_STYLE_INDENTGUIDE, FORE_COLOUR)
    TextArea.StyleSetBackground(wx.stc.STC_STYLE_INDENTGUIDE, BACK_COLOUR)

    

    #TextArea.MarginClick += TextArea_MarginClick;



    TextArea.SetFoldMarginColour(True, BACK_COLOUR)
    TextArea.SetFoldMarginHiColour(True, BACK_COLOUR)

    #Enable code folding
    TextArea.SetProperty("fold", "1")
    TextArea.SetProperty("fold.compact", "1")

    #Configure a margin to display folding symbols
    TextArea.SetMarginType(1, wx.stc.STC_MARGIN_SYMBOL)
    TextArea.SetMarginMask(1, wx.stc.STC_MASK_FOLDERS)
    TextArea.SetMarginSensitive(1, True)
    TextArea.SetMarginWidth(1, 20)
    
    
    #Configure folding markers with respective symbols
    TextArea.MarkerDefine(wx.stc.STC_MARKNUM_FOLDER, wx.stc.STC_MARK_CIRCLEPLUS if CODEFOLDING_CIRCULAR else wx.stc.STC_MARK_BOXPLUS, BACK_COLOUR, FORE_COLOUR)
    TextArea.MarkerDefine(wx.stc.STC_MARKNUM_FOLDEROPEN, wx.stc.STC_MARK_BOXPLUS if CODEFOLDING_CIRCULAR else wx.stc.STC_MARK_CIRCLEPLUS, BACK_COLOUR, FORE_COLOUR)
    TextArea.MarkerDefine(wx.stc.STC_MARKNUM_FOLDEREND, wx.stc.STC_MARK_CIRCLEPLUSCONNECTED if CODEFOLDING_CIRCULAR else wx.stc.STC_MARK_BOXPLUSCONNECTED, BACK_COLOUR, FORE_COLOUR)
    TextArea.MarkerDefine(wx.stc.STC_MARKNUM_FOLDERMIDTAIL, wx.stc.STC_MARK_TCORNER, BACK_COLOUR, FORE_COLOUR)
    TextArea.MarkerDefine(wx.stc.STC_MARKNUM_FOLDEROPENMID, wx.stc.STC_MARK_CIRCLEMINUSCONNECTED if CODEFOLDING_CIRCULAR else wx.stc.STC_MARK_BOXMINUSCONNECTED, BACK_COLOUR, FORE_COLOUR)
    TextArea.MarkerDefine(wx.stc.STC_MARKNUM_FOLDERSUB, wx.stc.STC_MARK_VLINE, BACK_COLOUR, FORE_COLOUR)
    TextArea.MarkerDefine(wx.stc.STC_MARKNUM_FOLDERTAIL, wx.stc.STC_MARK_LCORNER, BACK_COLOUR, FORE_COLOUR)

    #Enable automatic folding
    TextArea.SetAutomaticFold(wx.stc.STC_AUTOMATICFOLD_SHOW | wx.stc.STC_AUTOMATICFOLD_CLICK | wx.stc.STC_AUTOMATICFOLD_CHANGE)




def set_style(scintilla, lang):
    scintilla.SetLexerLanguage(lang)
    if not lang in scite_colors.styles:
        return
    style=scite_colors.styles[lang]
    if 'style' in style:
        for key,style_parts in style['style'].items():
            for p,q in style_parts.items():
                if p=='fore':
                    if isinstance(q, list):
                        scintilla.StyleSetForeground(int(key), wx.Colour(*q))
                    elif hasattr(wx, q.upper()):
                        scintilla.StyleSetForeground(int(key), getattr(wx, q.upper()))
                    
                elif p=='back':
                    
                    if isinstance(q, list):
                        scintilla.StyleSetBackground(int(key), wx.Colour(*q))
                    elif hasattr(wx, q.upper()):
                        scintilla.StyleSetBackground(int(key), getattr(wx, q.upper()))
                        
                    #print('background',key,q)
                elif p=='bold':
                    scintilla.StyleSetBold(int(key), q)
                    #print('bold',key,q)
                elif p=='italics':
                    scintilla.StyleSetItalic(int(key), q)
                    
                    
    if 'keywordclass' in style:
        scintilla.SetKeyWords(0, " ".join(style['keywordclass']))

class SourceFileFrame(SourceFileBase):
    def __init__(self, parent, name, path):
        super().__init__(parent)
        
        self.name = name
        self.path = path
        
        self.contents = open(path).read()
        
        
        if self.path.endswith('mml'):
            set_style(self.scintilla, 'yaml')
        elif self.path.endswith('mss'):
            set_style(self.scintilla, 'css')
            
        elif self.path.endswith('py'):
            set_style(self.scintilla, 'python')
        
        InitCodeFolding(self.scintilla)
        
        
        self.scintilla.StyleSetSpec(
            wx.stc.STC_STYLE_DEFAULT,
            "size:10,face:monospace")
        
        self.scintilla.SetText(self.contents)
        #self.scintilla.Colourise(0, -1)    
