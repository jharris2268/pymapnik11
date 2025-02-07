CODEFOLDING_CIRCULAR=False
BACK_COLOUR=wx.Colour(0x2a, 0x21, 0x1c)
FORE_COLOUR=wx.Colour(0xb7, 0xb7, 0xb7)


def InitCodeFolding(TextArea):

    TextArea.SetMarginType(1, wx.stc.STC_MARGIN_NUMBER)
    TextArea.SetMarginMask(1, 0)
    TextArea.SetMarginSensitive(1, True)
    
    TextArea.SetMarginWidth(1, 30)


    TextArea.StyleSetForeground(wx.stc.STC_STYLE_LINENUMBER, FORE_COLOUR)
    TextArea.StyleSetBackground(wx.stc.STC_STYLE_LINENUMBER, BACK_COLOUR)
    TextArea.StyleSetForeground(wx.stc.STC_STYLE_LINENUMBER, FORE_COLOUR)
    TextArea.StyleSetBackground(wx.stc.STC_STYLE_INDENTGUIDE, BACK_COLOUR)

    

    #TextArea.MarginClick += TextArea_MarginClick;



    #TextArea.SetFoldMarginColour(True, BACK_COLOUR)
    #TextArea.SetFoldMarginHiColour(True, BACK_COLOUR)

    #Enable code folding
    TextArea.SetProperty("fold", "1")
    TextArea.SetProperty("fold.compact", "1")

    #Configure a margin to display folding symbols
    TextArea.SetMarginType(2, wx.stc.STC_MARGIN_SYMBOL)
    TextArea.SetMarginMask(2, wx.stc.STC_MASK_FOLDERS)
    TextArea.SetMarginSensitive(2, True)
    TextArea.SetMarginWidth(2, 20)
    
    
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
